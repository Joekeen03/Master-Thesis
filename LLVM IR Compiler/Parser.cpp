#include "Parser.h"

#include <typeinfo>
#include <map>
#include <set>
#include <iterator>

#include "Tokens/TokenKeyword.h"
#include "Tokens/TokenOperator.h"
#include "Tokens/TokenString.h"
#include "Tokens/TokenGlobalNamedIdentifier.h"
#include "Tokens/TokenLocalNamedIdentifier.h"
#include "Tokens/TokenLocalUnnamedIdentifier.h"
#include "Tokens/TokenLocalIdentifier.h"
#include "Tokens/TokenParenthesis.h"
#include "Tokens/TokenCurlyBrace.h"
#include "Tokens/TokenTypeInteger.h"
#include "Tokens/TokenTypeReservedWord.h"
#include "Tokens/TokenNumberLiteral.h"
#include "Tokens/TokenComma.h"
#include "Tokens/TokenNamedMetadata.h"
#include "Tokens/TokenUnnamedMetadata.h"
#include "Tokens/TokenMetadataNodeStart.h"
#include "Tokens/TokenMetadataString.h"
#include "Tokens/TokenEOF.h"

#include "Types/TypeInteger.h"

#include "Expressions/ExpressionSourceFile.h"
#include "Expressions/ExpressionDataLayout.h"
#include "Expressions/ExpressionFunctionHeaderPreName.h"
#include "Expressions/ExpressionLocalNamedIdentifier.h"
#include "Expressions/ExpressionLocalUnnamedIdentifier.h"
#include "Expressions/ExpressionOperandIdentifier.h"
#include "Expressions/ExpressionOperandLiteralInteger.h"
#include "Expressions/ExpressionValueInteger.h"
#include "Expressions/ExpressionValueMetadataString.h"

#include "Expressions/Instructions/InstructionAlloca.h"
#include "Expressions/Instructions/InstructionStore.h"
#include "Expressions/Instructions/InstructionRetValue.h"
#include "Expressions/Instructions/YieldsValue.h"

#include "Lib/General.h"

namespace Parser {
    template <typename T>
    bool Parser::checkReserved(int pos, EnumRegistry::RegistryItem reserved) {
        auto token = (*tokens)[pos];
        return ((typeid(*token) == typeid(T))
                && (std::dynamic_pointer_cast<const T>(token)->registryItem == reserved));
    }
    
    template<typename T>
    std::string Parser::extractNamedIdentifier(int pos) {
        tokenPointer token = (*tokens)[pos];
        if (typeid(*token) == typeid(T)) {
            return std::dynamic_pointer_cast<const T>(token)->name;
        } else {
            throw ParsingException("Received incorrect identifier type at source position "+std::to_string(token->srcPos)+".", pos);
        }
    }

    stringExtractResult Parser::attemptExtractString(int pos) {
        auto token = getToken(pos);
        bool isString = Lib::isType<Tokens::TokenString>(token);
        return isString ? stringExtractResult(std::dynamic_pointer_cast<const Tokens::TokenString>(token)->str, true)
                        : stringExtractResult("", false);
    }

    numberExtractResult Parser::attemptExtractInteger(int pos) {
        auto token = getToken(pos);
        bool isInt = Lib::isType<Tokens::TokenNumberLiteral>(token);
        return isInt ? numberExtractResult(std::dynamic_pointer_cast<const Tokens::TokenNumberLiteral>(token)->val, true)
                        : numberExtractResult(-1, false);
    }

    std::string Parser::extractString(int pos) {
        tokenPointer token = getToken(pos);
        if (Lib::isType<Tokens::TokenString>(token)) {
            return std::dynamic_pointer_cast<const Tokens::TokenString>(token)->str;
        } else {
            throw ParsingException("Expected TokenString at token position "+std::to_string(token->srcPos)+", instead received: "+token->getName(), pos);
        }
    }

    int Parser::extractInteger(int pos) {
        tokenPointer token = getToken(pos);
        if (Lib::isType<Tokens::TokenNumberLiteral>(token)) {
            return std::dynamic_pointer_cast<const Tokens::TokenNumberLiteral>(token)->val;
        } else {
            throw ParsingException("Expected TokenInteger at source position "+std::to_string(token->srcPos)+", instead received: "+token->getName(), pos);
        }
    }

    template<typename T>
    bool Parser::isLeftToken(int pos) {
        tokenPointer token = (*tokens)[pos];
        return (typeid(*token) == typeid(T)
                && std::dynamic_pointer_cast<const T>(token)->left);
    }

    template<typename T>
    bool Parser::isRightToken(int pos) {
        tokenPointer token = (*tokens)[pos];
        return (typeid(*token) == typeid(T)
                && !std::dynamic_pointer_cast<const T>(token)->left);
    }

    /*** PARSE METHODS ***/
    
    /** INSTRUCTION PARSE METHODS **/

    // Instructions that yield void

    Instructions::InstructionParseResult Parser::parseInstructionStore(int startPos) {
        int currPos = startPos;
        bool success = false;
        std::shared_ptr<const Types::TypeSized> valueType; // FIXME should be some kind of TypeFirstClass
        std::shared_ptr<const Expressions::ExpressionOperand> valueOperand;
        std::shared_ptr<const Expressions::ExpressionIdentifier> pointerOperand;
        int alignment = -1;
        if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::store)) {
            currPos++;
            // Try parsing volatile
            // TODO
            
            auto typeResult = parseFirstClassKnownSizeType(currPos);
            if (typeResult.success) {
                valueType = typeResult.result;
                currPos = typeResult.newPos;
                auto valueOperandResult = parseOperand(currPos);
                if (valueOperandResult.success) {
                    valueOperand = valueOperandResult.result;
                    currPos = valueOperandResult.newPos;
                    if (Lib::isType<Tokens::TokenComma>(getToken(currPos))) {
                        currPos++;
                        if (checkReserved<Tokens::TokenTypeReservedWord>(currPos, TypeReservedWords::typePtr)) {
                            currPos++;
                            auto pointerOperandResult = parseIdentifier(currPos);
                            if (pointerOperandResult.success) {
                                pointerOperand = pointerOperandResult.result;
                                currPos = pointerOperandResult.newPos;
                                // Try to parse alignment:
                                if (Lib::isType<Tokens::TokenComma>(getToken(currPos)) && checkReserved<Tokens::TokenKeyword>(currPos+1, ReservedWords::align)) {
                                    currPos += 2;
                                    alignment = extractInteger(currPos);
                                    currPos++;
                                }
                                
                                // Try to parse nontemporal metadata:
                                // TODO

                                //Try to parse invariant group metadata:
                                // TODO
                                success = true;
                            }
                        }
                    }
                }
            }
        }
        return success ? Instructions::InstructionParseResult(std::make_shared<const Instructions::InstructionStore>
                                                                (valueType, valueOperand, pointerOperand, alignment), currPos)
                        : Instructions::InstructionParseResult();
    }

    // Instructions that yield values

    Instructions::InstructionParseResult Parser::parseInstructionAlloca(int startPos, std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assignee) {
        int currPos = startPos;
        bool success = false;
        std::shared_ptr<const Types::TypeSized> allocationType;
        bool inalloca = false;
        int alignment = -1;
        if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::alloca)) {
            currPos++;
            // Try parsing an 'inalloca' token
            // TODO

            Lib::ResultPointer<Types::TypeSized> typeResult = parseSizedType(currPos);
            if (typeResult.success) {
                currPos++;
                allocationType = typeResult.result;
                // Try parsing the <ty> section
                // TODO

                // Try parsing the alignment section
                if (Lib::isType<Tokens::TokenComma>(getToken(currPos)) && checkReserved<Tokens::TokenKeyword>(currPos+1, ReservedWords::align)) {
                    currPos += 2;
                    alignment = extractInteger(currPos);
                    currPos++;
                }

                // Try parsing the address space section.
                // TODO
                success = true;
            }
        }
        return success ? Instructions::InstructionParseResult(std::make_shared<Instructions::InstructionAlloca>
                                                                (assignee, inalloca, allocationType, alignment), currPos)
                        : Instructions::InstructionParseResult();
    }

    // Terminator Instructions

    Instructions::InstructionParseResult Parser::parseInstructinRetValue(int startPos) {
        bool success = false;
        int currPos = startPos;
        int nextPos = -1;
        std::shared_ptr<const Types::Type> retType;
        std::shared_ptr<const Expressions::ExpressionOperand> retVal;

        if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::ret)) {
            currPos++;
            auto typeParseResult = parseType(currPos); // FIXME Should be restricted to valid return types
            if (typeParseResult.success) {
                retType = typeParseResult.result;
                currPos = typeParseResult.newPos;
                auto valueParseResult = parseOperand(currPos);
                if (valueParseResult.success) {
                    retVal = valueParseResult.result;
                    nextPos = valueParseResult.newPos;
                    success = true;
                }
            }
        }
        return success ? Instructions::InstructionParseResult(std::make_shared<const Instructions::InstructionRetValue>(retType, retVal), nextPos)
                        : Instructions::InstructionParseResult();
    }

    /** GENERAL PARSE METHODS **/

    Lib::ResultPointer<Expressions::ExpressionIdentifier> Parser::parseIdentifier(int startPos) {
        bool success = false;
        std::shared_ptr<const Expressions::ExpressionIdentifier> identifier;
        int nextPos;
        auto localIdentifierResult = parseLocalIdentifier(startPos);
        if (localIdentifierResult.success) {
            identifier = localIdentifierResult.result;
            nextPos = localIdentifierResult.newPos;
            success = true;
        }
        return success ? Lib::ResultPointer<Expressions::ExpressionIdentifier>(identifier, nextPos)
                        : Lib::ResultPointer<Expressions::ExpressionIdentifier>();
    }

    Lib::ResultPointer<Expressions::ExpressionLocalIdentifier> Parser::parseLocalIdentifier(int startPos) {
        tokenPointer token = getToken(startPos);
        int nextPos = startPos+1;
        bool success = false;
        std::shared_ptr<const Expressions::ExpressionLocalIdentifier> identifier;
        // FIXME Implement proper hierarchy for identifiers
        if (Lib::isType<Tokens::TokenLocalUnnamedIdentifier>(token)) {
            identifier = std::make_shared<const Expressions::ExpressionLocalUnnamedIdentifier>(std::dynamic_pointer_cast<const Tokens::TokenLocalUnnamedIdentifier>(token)->ID);
            success = true;
        } else if (Lib::isType<Tokens::TokenLocalNamedIdentifier>(token)) {
            identifier = std::make_shared<const Expressions::ExpressionLocalNamedIdentifier>(std::dynamic_pointer_cast<const Tokens::TokenLocalNamedIdentifier>(token)->name);
            success = true;
        }
        return success ? Lib::ResultPointer<Expressions::ExpressionLocalIdentifier>(identifier, nextPos)
                        : Lib::ResultPointer<Expressions::ExpressionLocalIdentifier>();
    }

    Lib::ResultPointer<Types::Type> Parser::parseType(int startPos) {
        int nextPosition=-1;
        std::shared_ptr<const Tokens::Token> firstToken = getToken(startPos);
        std::shared_ptr<const Types::Type> determinedType;
        bool success = false;
        if (Lib::isType<Tokens::TokenTypeInteger>(firstToken)) {
            determinedType = std::make_shared<const Types::TypeInteger>(std::dynamic_pointer_cast<const Tokens::TokenTypeInteger>(firstToken)->bitWidth);
            success = true;
            nextPosition = startPos+1;
        }
        return success ? Lib::ResultPointer<Types::Type>(determinedType, nextPosition) : Lib::ResultPointer<Types::Type>();
    }

    Lib::ResultPointer<Types::TypeSized> Parser::parseSizedType(int startPos) {
        int nextPosition=-1;
        std::shared_ptr<const Tokens::Token> firstToken = getToken(startPos);
        std::shared_ptr<const Types::TypeSized> determinedType;
        bool success = false;
        if (Lib::isType<Tokens::TokenTypeInteger>(firstToken)) {
            determinedType = std::make_shared<const Types::TypeInteger>(std::dynamic_pointer_cast<const Tokens::TokenTypeInteger>(firstToken)->bitWidth);
            success = true;
            nextPosition = startPos+1;
        }
        return success ? Lib::ResultPointer<Types::TypeSized>(determinedType, nextPosition) : Lib::ResultPointer<Types::TypeSized>();
    }

    Lib::ResultPointer<Types::TypeSized> Parser::parseFirstClassKnownSizeType(int startPos) {
        int nextPosition=-1;
        std::shared_ptr<const Tokens::Token> firstToken = getToken(startPos);
        std::shared_ptr<const Types::TypeSized> determinedType;
        bool success = false;
        if (Lib::isType<Tokens::TokenTypeInteger>(firstToken)) {
            determinedType = std::make_shared<const Types::TypeInteger>(std::dynamic_pointer_cast<const Tokens::TokenTypeInteger>(firstToken)->bitWidth);
            success = true;
            nextPosition = startPos+1;
        }
        return success ? Lib::ResultPointer<Types::TypeSized>(determinedType, nextPosition) : Lib::ResultPointer<Types::TypeSized>();
    }

    Lib::ResultPointer<Expressions::ExpressionOperand> Parser::parseOperand(int startPos) {
        int currPos = startPos;
        int nextPos = -1;
        bool success = false;
        std::shared_ptr<const Expressions::ExpressionOperand> operand;
        auto numberResult = attemptExtractInteger(currPos);
        if (numberResult.second) {
            success = true;
            operand = std::make_shared<const Expressions::ExpressionOperandLiteralInteger>(numberResult.first);
            nextPos = currPos+1;
        } else {
            auto identifierResult = parseIdentifier(currPos);
            if (identifierResult.success) {
                success = true;
                operand = std::make_shared<const Expressions::ExpressionOperandIdentifier>(identifierResult.result);
                nextPos = identifierResult.newPos;
            }
        }
        return success ? Lib::ResultPointer<Expressions::ExpressionOperand>(operand, nextPos)
                        : Lib::ResultPointer<Expressions::ExpressionOperand>();
    }

    ParsingResult<Expressions::ExpressionSourceFile> Parser::parseSourceFile(int startPos) {
        int currPos = startPos;
        int nextTokenAfterExpr = -1;
        std::string str = "";
        bool success = false;
        if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::source_filename)) {
            currPos++;
            if (checkReserved<Tokens::TokenOperator>(currPos, Operators::equals)) {
                currPos++;
                str = extractString(currPos);
                success = true;
                nextTokenAfterExpr = currPos+1;
            }
        }
        return success ? ParsingResult<Expressions::ExpressionSourceFile>(std::make_shared<Expressions::ExpressionSourceFile>(str), nextTokenAfterExpr)
                        : ParsingResult<Expressions::ExpressionSourceFile>();
    }

    ParsingResult<Expressions::ExpressionDataLayout> Parser::parseDataLayout(int startPos) {
        
        int currPos = startPos;
        int nextTokenAfterExpr = currPos;
        std::string str = "";
        bool success = false;
        if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::target)) {
            currPos++;
            if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::datalayout)) {
                currPos++;
                if (checkReserved<Tokens::TokenOperator>(currPos, Operators::equals)) {
                    currPos++;
                    // FIXME Was it better to use the attemptExtractString(...) instead?
                    str = extractString(currPos);
                    success = true;
                    nextTokenAfterExpr = currPos+1;
                }
            }
            
        }
        return success ? ParsingResult<Expressions::ExpressionDataLayout>(std::make_shared<Expressions::ExpressionDataLayout>(str), nextTokenAfterExpr)
                        : ParsingResult<Expressions::ExpressionDataLayout>();
    }

    ParsingResult<Expressions::ExpressionTargetTriple> Parser::parseTargetTriple(int startPos) {
        
        int currPos = startPos;
        int nextTokenAfterExpr = currPos;
        std::string str = "";
        bool success = false;
        // FIXME Should it throw a ParsingException if any of the other if (...) statements fail?
        if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::target)) {
            currPos++;
            if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::triple)) {
                currPos++;
                if (checkReserved<Tokens::TokenOperator>(currPos, Operators::equals)) {
                    currPos++;
                    str = extractString(currPos);
                    success = true;
                    nextTokenAfterExpr = currPos+1;
                }
            }
            
        }
        return success ? ParsingResult<Expressions::ExpressionTargetTriple>(std::make_shared<Expressions::ExpressionTargetTriple>(str), nextTokenAfterExpr)
                        : ParsingResult<Expressions::ExpressionTargetTriple>();
    }
    
    ParsingResult<Expressions::ExpressionFunctionHeaderPreName> Parser::parseFunctionHeaderPreName(int startPos) {
        
        std::shared_ptr<std::vector<std::shared_ptr<const Tokens::TokenKeyword>>> keywords = std::make_shared<std::vector<std::shared_ptr<const Tokens::TokenKeyword>>>();
        int currPos = startPos;
        // Parse linkages:

        // Parse Runtime Preemption Specifiers - need to add dso_preemptable
        if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::dso_local)) {
            keywords->push_back(std::dynamic_pointer_cast<const Tokens::TokenKeyword>((*tokens)[currPos]));
            currPos++;
        }

        // Visibility Encoding:

        // DLL Storage Class:

        // Calling Convention:

        return ParsingResult<Expressions::ExpressionFunctionHeaderPreName>(std::make_shared<Expressions::ExpressionFunctionHeaderPreName>(keywords), currPos);
    }

    ParsingResult<Expressions::ExpressionReturnType> Parser::parseFunctionReturnType(int startPos) {
        int currPos = startPos;
        bool success = false;
        std::shared_ptr<std::vector<std::shared_ptr<const Tokens::TokenKeyword>>> attributes
            = std::make_shared<std::vector<std::shared_ptr<const Tokens::TokenKeyword>>>();
        if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::noundef)) {
            attributes->push_back(std::dynamic_pointer_cast<const Tokens::TokenKeyword>((*tokens)[currPos]));
            currPos++;
        }
        auto typeResult = parseType(currPos);
        if (typeResult.success) {
            currPos = typeResult.newPos;
            success = true;
        }
        
        return success ? ParsingResult<Expressions::ExpressionReturnType>(std::make_shared<Expressions::ExpressionReturnType>(attributes, typeResult.result), currPos)
                        : ParsingResult<Expressions::ExpressionReturnType>();
    }

    ParsingResult<Expressions::ExpressionArgumentList> Parser::parseFunctionArgumentList(int startPos) {
        // FIXME Unnamed function parameters should count towards unnamed temporary numbering (e.g. first unnamed parameter
        //  should be %0)
        bool success = false;
        int currPos = startPos;
        if (isLeftToken<Tokens::TokenParenthesis>(currPos)) {
            currPos++;
            // TODO Capture arguments
            if (isRightToken<Tokens::TokenParenthesis>(currPos)) {
                currPos++;
                success = true;
            }
        }
        return success ? ParsingResult<Expressions::ExpressionArgumentList>(std::make_shared<Expressions::ExpressionArgumentList>(), currPos)
                        : ParsingResult<Expressions::ExpressionArgumentList>();
    }

    ParsingResult<Expressions::ExpressionFunctionHeaderPostName> Parser::parseFunctionHeaderPostName(int startPos) {
        std::shared_ptr<std::vector<std::shared_ptr<const Tokens::TokenKeyword>>> keywords = std::make_shared<std::vector<std::shared_ptr<const Tokens::TokenKeyword>>>();
        std::shared_ptr<std::map<std::string, std::string>> strKeywords = std::make_shared<std::map<std::string, std::string>>();
        int currPos = startPos;
        // Parse unamed_addr:

        // Parse address space:

        // Parse function attributes:
        bool parseFunctionAttribute = true;
        while (parseFunctionAttribute && currPos < tokens->size()) {
            tokenPointer token = (*tokens)[currPos];
            if (typeid(*token) == typeid(Tokens::TokenKeyword)) {
                std::shared_ptr<const Tokens::TokenKeyword> keywordToken = std::dynamic_pointer_cast<const Tokens::TokenKeyword>(token);
                EnumRegistry::RegistryItem keyword = keywordToken->registryItem;
                if (keyword == ReservedWords::mustprogress || keyword == ReservedWords::noinline || keyword == ReservedWords::norecurse
                    || keyword == ReservedWords::nounwind || keyword == ReservedWords::optnone || keyword == ReservedWords::uwtable) {
                        keywords->push_back(keywordToken);
                        currPos++;
                }
            } else {
                stringExtractResult strLabelResult = attemptExtractString(currPos);
                if (strLabelResult.second) { // Should this verify that the string value is an accepted keyword?
                    if (checkReserved<Tokens::TokenOperator>(currPos+1, Operators::equals)) {
                        std::string strValue = extractString(currPos+2);
                        (*strKeywords)[strLabelResult.first] = strValue;
                        currPos += 3;
                    } // What if this fails?
                } else {
                    parseFunctionAttribute = false;
                }
            }
        }

        // Parse section name (?):

        // Parse partition name (?):

        // Parse comdat:

        // Parse garbage collector name:
        
        // Parse prefix:

        // Parse prologue:

        // Parse personality:

        // Parse attached metadata:

        return ParsingResult<Expressions::ExpressionFunctionHeaderPostName>(std::make_shared<Expressions::ExpressionFunctionHeaderPostName>(keywords, strKeywords), currPos);
    }

    Instructions::InstructionParseResult Parser::parseInstruction(int startPos) {
        auto localIdentifierResult = parseLocalIdentifier(startPos);
        int currPos = startPos;
        if (localIdentifierResult.success) {
            currPos = localIdentifierResult.newPos;
            if (checkReserved<Tokens::TokenOperator>(currPos, Operators::equals)) {
                currPos++;
                for (int i = 0; i < nInstructionsYieldValue; i++)
                {
                    auto valueInstructionParser = instructionsYieldValue[i];
                    auto result = (this->*valueInstructionParser)(currPos, localIdentifierResult.result);
                    if (result.success) return result;
                }
            } else {    
                auto currToken = getToken(currPos);
                throw ParsingException("Expected TokenOperator(equals) at source position "+std::to_string(currToken->srcPos)
                                        +", received "+currToken->getNameAndPos()+".", currPos);
            }
        } else {
            for (int i = 0; i < nInstructionsYieldVoid; i++)
            {
                auto voidInstructionParser = instructionsYieldVoid[i];
                auto result = (this->*voidInstructionParser)(currPos);
                if (result.success) return result;
            }
        }
        return Instructions::InstructionParseResult();
    }

    CodeBlockParsingResult Parser::parseFunctionCodeBlock(int startPos, int startUnnamedLocal, std::shared_ptr<std::set<std::string>> localNameSet) {
        int nextUnnamedLocal = startUnnamedLocal;
        int currPos = startPos;
        std::string label;
        auto newLocalNameSet = std::make_shared<std::set<std::string>>();
        std::copy(newLocalNameSet->begin(), newLocalNameSet->end(), std::inserter(*localNameSet, localNameSet->begin()));
        auto instructions = std::make_shared<std::vector<const std::shared_ptr<const Instructions::Instruction>>>();
        std::shared_ptr<const Instructions::InstructionTerminator> terminator;
        
        if (false) {

        } else { // No label
            label = std::to_string(nextUnnamedLocal);
            nextUnnamedLocal++;
        }
        
        // Parse code
        bool parseInstructions = true;
        bool foundTerminator = false;
        
        do {
            auto instructionResult = parseInstruction(currPos);
            if (instructionResult.success) {
                if (Lib::isType<Instructions::YieldsValue>(instructionResult.result)) {
                    // Ensure the local assigned to isn't a duplicate assignment (violating SSA), and if it's an unnamed local,
                    //  that it matches with the next unnamed local ID (local ID definitions must be monotonically increasing, as
                    //      in %0=, %1=, %2=, %3=, ...)
                    auto assignedIdentifier = std::dynamic_pointer_cast<const Instructions::YieldsValue>(instructionResult.result)->assignee;
                    if (Lib::isType<Expressions::ExpressionLocalUnnamedIdentifier>(assignedIdentifier)) {
                        auto identifier = std::dynamic_pointer_cast<const Expressions::ExpressionLocalUnnamedIdentifier>(assignedIdentifier);
                        if (identifier->ID < nextUnnamedLocal) {
                            throw ParsingException("LLVM IR is a single-assignment language. Duplicate unnamed local assignment at source position "
                                                    +std::to_string(getToken(currPos)->srcPos)+".", currPos);
                        } else if (identifier->ID > nextUnnamedLocal) {
                            throw ParsingException("Unnamed locals must be defined consecutively - 1, 2, 3...; local definition at sourcePosition"
                                                    +std::to_string(getToken(currPos)->srcPos)+" was not the expected ID, "
                                                    +std::to_string(nextUnnamedLocal)+".", currPos);
                        }
                        newLocalNameSet->insert(std::to_string(identifier->ID));
                    } else if (Lib::isType<Expressions::ExpressionLocalNamedIdentifier>(assignedIdentifier)) {
                        auto identifier = std::dynamic_pointer_cast<const Expressions::ExpressionLocalNamedIdentifier>(assignedIdentifier);
                        if (newLocalNameSet->count(identifier->name)) {
                            throw ParsingException("LLVM IR is a single-assignment language. Duplicate named local assignment at source position "
                                                    +std::to_string(getToken(currPos)->srcPos)+".", currPos);
                        }
                        newLocalNameSet->insert(identifier->name);
                    }
                }
                if (Lib::isDerivedType<Instructions::InstructionTerminator>(instructionResult.result)) {
                    terminator = std::dynamic_pointer_cast<const Instructions::InstructionTerminator>(instructionResult.result);
                    parseInstructions = false;
                    foundTerminator = true;
                } else {
                    instructions->push_back(instructionResult.result);
                }
                currPos = instructionResult.newPos;
            } else {
                parseInstructions = false;
            }

        } while (parseInstructions);
        return foundTerminator ? CodeBlockParsingResult(std::make_shared<Expressions::ExpressionFunctionCodeBlock>(label, instructions, terminator),
                                                        currPos, nextUnnamedLocal, newLocalNameSet)
                                : CodeBlockParsingResult();
    }

    Lib::ResultPointer<std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock>>> Parser::parseFunctionCodeBlocks(int startPos) {
        int currPos = startPos;
        bool success = false;
        auto codeBlocks = std::make_shared<std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock>>>();
        auto localNameSet = std::make_shared<std::set<std::string>>();
        if (isLeftToken<Tokens::TokenCurlyBrace>(currPos)) { // Function code section should open with a '{'
            currPos++;
            int currUnnamedLocal = 0;
            bool parseBlocks = true;
            bool notEmpty = false;
            do { // Build list of code blocks
                auto parseResult = parseFunctionCodeBlock(currPos, currUnnamedLocal, localNameSet);
                if (parseResult.success) {
                    codeBlocks->push_back(parseResult.result);
                    currPos = parseResult.newPos;
                    currUnnamedLocal = parseResult.nextUnnamedLocal;
                    localNameSet = parseResult.localNameSet;
                    notEmpty = true;
                } else {
                    parseBlocks = false;
                }
            } while (parseBlocks);

            if (notEmpty) { // Must contain at least one code block.
                if (isRightToken<Tokens::TokenCurlyBrace>(currPos)) { // Function code section closes with a '}'
                    currPos++;
                    success = true;
                }
            }
        }
        return success ? Lib::ResultPointer<std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock>>>(codeBlocks, currPos)
                        : Lib::ResultPointer<std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock>>>();
    }

    Lib::ResultPointer<Expressions::ExpressionFunctionDefinition> Parser::parseFunctionDefinition(int startPos) {
        bool success = false;
        int currPos = startPos;
        int nextPos = -1;

        std::shared_ptr<const Expressions::ExpressionFunctionHeaderPreName> headerPreName;
        std::shared_ptr<const Expressions::ExpressionReturnType> returnType;
        std::string functionName;
        std::shared_ptr<const Expressions::ExpressionArgumentList> parameterList;
        std::shared_ptr<const Expressions::ExpressionFunctionHeaderPostName> headerPostName;
        std::shared_ptr<const std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock>>> codeBlocks;

        if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::define)) {
            currPos++;
            auto headerPreNameResult = parseFunctionHeaderPreName(currPos);
            if (headerPreNameResult.success) {
                headerPreName = headerPreNameResult.result;
                currPos = headerPreNameResult.newPos;
                auto returnTypeResult = parseFunctionReturnType(currPos);
                if (returnTypeResult.success) {
                    returnType = returnTypeResult.result;
                    currPos = returnTypeResult.newPos;
                    std::string functionName = extractNamedIdentifier<Tokens::TokenGlobalNamedIdentifier>(currPos);
                    currPos++;
                    auto parameterListResult = parseFunctionArgumentList(currPos);
                    if (parameterListResult.success) {
                        parameterList = parameterListResult.result;
                        currPos = parameterListResult.newPos;
                        auto headerPostNameResult = parseFunctionHeaderPostName(currPos);
                        if (headerPostNameResult.success) {
                            headerPostName = headerPostNameResult.result;
                            currPos = headerPostNameResult.newPos;
                            auto codeBlocksResult = parseFunctionCodeBlocks(currPos);
                            if (codeBlocksResult.success) {
                                codeBlocks = codeBlocksResult.result;
                                nextPos = codeBlocksResult.newPos;
                                success = true;
                            }
                        }
                    }
                }
            }
        }
        auto functionDefinition = std::make_shared<Expressions::ExpressionFunctionDefinition>(headerPreName, returnType, functionName, parameterList, headerPostName, codeBlocks);
        return success ? Lib::ResultPointer<Expressions::ExpressionFunctionDefinition>(functionDefinition, nextPos)
                        : Lib::ResultPointer<Expressions::ExpressionFunctionDefinition>();
    }

    // Metadata parsing methods

    
    ParsingResult<Expressions::ExpressionNamedMetadataIdentifier> Parser::parseNamedMetadataIdentifier(int startPos) {
        auto token = getToken(startPos);
        return Lib::isType<Tokens::TokenNamedMetadata>(token)
            ? ParsingResult<Expressions::ExpressionNamedMetadataIdentifier>(std::make_shared<Expressions::ExpressionNamedMetadataIdentifier>(std::dynamic_pointer_cast<const Tokens::TokenNamedMetadata>(token)->name), startPos+1)
            : ParsingResult<Expressions::ExpressionNamedMetadataIdentifier>();
    }

    ParsingResult<Expressions::ExpressionUnnamedMetadataIdentifier> Parser::parseUnnamedMetadataIdentifier(int startPos) {
        auto token = getToken(startPos);
        return Lib::isType<Tokens::TokenUnnamedMetadata>(token)
            ? ParsingResult<Expressions::ExpressionUnnamedMetadataIdentifier>(std::make_shared<Expressions::ExpressionUnnamedMetadataIdentifier>(std::dynamic_pointer_cast<const Tokens::TokenUnnamedMetadata>(token)->ID), startPos+1)
            : ParsingResult<Expressions::ExpressionUnnamedMetadataIdentifier>();
    }

    ParsingResult<Expressions::ExpressionNamedMetadataDefinition> Parser::parseNamedMetdataDefinition(int startPos) {
        // FIXME Does metadata follow the SSA rules, and should this enforce them?
        //  I.e. should this throw an error if a duplicate metadata definition is encountered?
        bool success = false;
        int currPos = startPos;
        int nextPos = -1;
        std::shared_ptr<const Expressions::ExpressionNamedMetadataIdentifier> assignee;
        std::shared_ptr<std::vector<const std::shared_ptr<const Expressions::ExpressionUnnamedMetadataIdentifier>>> values;
        auto namedIdentifierResult = parseNamedMetadataIdentifier(currPos);
        if (namedIdentifierResult.success) {
            currPos = namedIdentifierResult.newPos;
            assignee = namedIdentifierResult.result;
            if (checkReserved<Tokens::TokenOperator>(currPos, Operators::equals)) {
                currPos++;
                if (Lib::isType<Tokens::TokenMetadataNodeStart>(getToken(currPos))) {
                    currPos++;
                    auto firstUnnamedIdentifierResult = parseUnnamedMetadataIdentifier(currPos);
                    if (firstUnnamedIdentifierResult.success) {
                        values = std::make_shared<std::vector<const std::shared_ptr<const Expressions::ExpressionUnnamedMetadataIdentifier>>>();
                        values->push_back(firstUnnamedIdentifierResult.result);
                        currPos = firstUnnamedIdentifierResult.newPos;
                        bool buildValuesVector = true;
                        do {
                            if (Lib::isType<Tokens::TokenComma>(getToken(currPos))) {
                                currPos++;
                                auto unnamedIdentifierResult = parseUnnamedMetadataIdentifier(currPos);
                                if (unnamedIdentifierResult.success) {
                                    values->push_back(unnamedIdentifierResult.result);
                                    currPos = unnamedIdentifierResult.newPos;
                                } else {
                                    buildValuesVector = false;
                                }
                            } else {
                                buildValuesVector = false;
                            }
                        } while (buildValuesVector);
                        if (isRightToken<Tokens::TokenCurlyBrace>(currPos)) {
                            currPos++;
                            nextPos = currPos;
                            success = true;
                        }
                    }
                }
            }
        }
        auto constVec = Lib::makePointerToConst(values);
        return success ? ParsingResult<Expressions::ExpressionNamedMetadataDefinition>(std::make_shared<Expressions::ExpressionNamedMetadataDefinition>(assignee, constVec), nextPos)
                        : ParsingResult<Expressions::ExpressionNamedMetadataDefinition>();
    }

    ParsingResult<Expressions::ExpressionValue> Parser::parseValue(int startPos) {
        int currPos = startPos;
        int nextPos = -1;
        bool success = false;
        auto firstToken = getToken(currPos);
        std::shared_ptr<const Expressions::ExpressionValue> val;
        if (Lib::isType<Tokens::TokenMetadataString>(firstToken)) {
            val = std::make_shared<const Expressions::ExpressionValueMetadataString>(std::dynamic_pointer_cast<const Tokens::TokenMetadataString>(firstToken)->str);
            nextPos = currPos+1;
            success = true;
        } else if (Lib::isType<Tokens::TokenTypeInteger>(firstToken)) {
            currPos++;
            auto intType = std::make_shared<const Types::TypeInteger>(std::dynamic_pointer_cast<const Tokens::TokenTypeInteger>(firstToken)->bitWidth);
            auto valToken = getToken(currPos);
            if (Lib::isType<Tokens::TokenNumberLiteral>(valToken)) {
                val = std::make_shared<const Expressions::ExpressionValueInteger>(intType, std::dynamic_pointer_cast<const Tokens::TokenNumberLiteral>(valToken)->val);
                nextPos = currPos+1;
                success = true;
            }
        }
        return success ? ParsingResult<Expressions::ExpressionValue>(val, nextPos) : ParsingResult<Expressions::ExpressionValue>();
    }

    ParsingResult<Expressions::ExpressionUnnamedMetadataDefinition> Parser::parseUnnamedMetdataDefinition(int startPos) {
        // FIXME Should this enforce that unnamed metadata should follow the same sequential numbering scheme as unnamed locals?
        //  E.g. first unnamed metadata must be !0, second must be !1, etc.
        //  Also, can you define metadata nodes w/o explicitly specifying their ID? E.g.:
        //      !{"asdf"}
        //  instead of
        //      !0 = !{"asdf"}
        bool success = false;
        int currPos = startPos;
        int nextPos = -1;
        std::shared_ptr<const Expressions::ExpressionUnnamedMetadataIdentifier> assignee;
        bool distinct = false;
        std::shared_ptr<std::vector<const std::shared_ptr<const Expressions::ExpressionValue>>> values;
        auto unnamedIdentifierResult = parseUnnamedMetadataIdentifier(currPos);
        if (unnamedIdentifierResult.success) {
            currPos = unnamedIdentifierResult.newPos;
            assignee = unnamedIdentifierResult.result;
            if (checkReserved<Tokens::TokenOperator>(currPos, Operators::equals)) {
                currPos++;
                // Attempt to parse "distinct"
                if (Lib::isType<Tokens::TokenMetadataNodeStart>(getToken(currPos))) {
                    currPos++;
                    auto firstValueResult = parseValue(currPos);
                    if (firstValueResult.success) {
                        values = std::make_shared<std::vector<const std::shared_ptr<const Expressions::ExpressionValue>>>();
                        values->push_back(firstValueResult.result);
                        currPos = firstValueResult.newPos;
                        bool buildValuesVector = true;
                        do {
                            if (Lib::isType<Tokens::TokenComma>(getToken(currPos))) {
                                currPos++;
                                auto valueResult = parseValue(currPos);
                                if (valueResult.success) {
                                    values->push_back(valueResult.result);
                                    currPos = valueResult.newPos;
                                } else {
                                    buildValuesVector = false;
                                }
                            } else {
                                buildValuesVector = false;
                            }
                        } while (buildValuesVector);
                        if (isRightToken<Tokens::TokenCurlyBrace>(currPos)) {
                            currPos++;
                            nextPos = currPos;
                            success = true;
                        }
                    }
                }
            }
        }
        if (success) {
            auto constVec = Lib::makePointerToConst(values);
            auto definitionExpression = std::make_shared<Expressions::ExpressionUnnamedMetadataDefinition>(assignee, distinct, constVec);
            return ParsingResult<Expressions::ExpressionUnnamedMetadataDefinition>(definitionExpression, nextPos);
        } else {
            return ParsingResult<Expressions::ExpressionUnnamedMetadataDefinition>();
        }
    }

    // Misc

    std::shared_ptr<const Expressions::ExpressionFile> Parser::parse() {
        
        bool success = false;
        int pos = 0;
        
        std::shared_ptr<const Expressions::ExpressionSourceFile> sourceFile;
        std::shared_ptr<const Expressions::ExpressionDataLayout> dataLayout;
        std::shared_ptr<const Expressions::ExpressionTargetTriple> targetTriple;
        auto functionDefinitions = std::make_shared<std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionDefinition>>>();
        auto namedMetadataDefinitions = std::make_shared<std::vector<const std::shared_ptr<const Expressions::ExpressionNamedMetadataDefinition>>>();
        auto unnamedMetadataDefinitions = std::make_shared<std::vector<const std::shared_ptr<const Expressions::ExpressionUnnamedMetadataDefinition>>>();

        if (auto srcResult = parseSourceFile(pos); srcResult.success) {
            sourceFile = srcResult.result;
            pos = srcResult.newPos;
        } else {
            throw ParsingException("Missing source filename at start of file.", pos);
        }
        // The datalayout and target triples are optional.
        //  Should I have their parse methods throw ParsingExceptions?
        //  Maybe the different parse...(...) methods should throw ParsingExceptions if they can't successfully parse their intended
        //  expression, and the function that calls them is responsible for determining whether or not to ignore them?
        // Are these required to be at the start of the file?
        // FIXME How to handle out of bounds array accesses (go off the end of the array while parsing something)?
        
        if (auto dataLayoutResult = parseDataLayout(pos); dataLayoutResult.success) {
            dataLayout = dataLayoutResult.result;
            pos = dataLayoutResult.newPos;
        }

        if (auto targetTripleResult = parseTargetTriple(pos); targetTripleResult.success) {
            targetTriple = targetTripleResult.result;
            pos = targetTripleResult.newPos;
        }

        while (!Lib::isType<Tokens::TokenEOF>(getToken(pos))) {
            std::cout<< "a";
            if (auto functionDefinitionResult = parseFunctionDefinition(pos); functionDefinitionResult.success) {
                functionDefinitions->push_back(functionDefinitionResult.result);
                pos = functionDefinitionResult.newPos;
            } else if (auto namedMetadataResult = parseNamedMetdataDefinition(pos); namedMetadataResult.success) {
                namedMetadataDefinitions->push_back(namedMetadataResult.result);
                pos = namedMetadataResult.newPos;
            } else if (auto unnamedMetadataResult = parseUnnamedMetdataDefinition(pos); unnamedMetadataResult.success) {
                unnamedMetadataDefinitions->push_back(unnamedMetadataResult.result);
                pos = unnamedMetadataResult.newPos;
            } else {
                auto token = getToken(pos);
                outputToken(pos);
                outputToken(pos+1);
                outputToken(pos+2);
                throw ParsingException("Unrecognized module-level structure start token: "+token->getNameAndPos()+".", pos);
            }
            std::cout<< "b";
        }
        
        std::shared_ptr<const Expressions::ExpressionFile> result
                    = std::make_shared<const Expressions::ExpressionFile>(sourceFile, dataLayout, targetTriple,
                                                                            Lib::makePointerToConst(functionDefinitions),
                                                                            Lib::makePointerToConst(unnamedMetadataDefinitions),
                                                                            Lib::makePointerToConst(namedMetadataDefinitions));
        
        return result;
    }

    const instructionYieldsVoidParser instructionsYieldVoid[] = {
        // Terminator Instructions:
        &Parser::parseInstructinRetValue,

        // Normal Instructions:
        &Parser::parseInstructionStore,
    };
    const int nInstructionsYieldVoid = sizeof(instructionsYieldVoid)/sizeof(instructionYieldsVoidParser);
    const instructionYieldsValueParser instructionsYieldValue[] = {
        &Parser::parseInstructionAlloca,
    };
    const int nInstructionsYieldValue = sizeof(instructionsYieldValue)/sizeof(instructionYieldsValueParser);
}