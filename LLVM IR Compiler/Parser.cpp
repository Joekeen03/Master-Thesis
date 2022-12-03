#include "Parser.h"

#include <typeinfo>
#include <map>
#include <set>
#include <iterator>

#include "Tokens/TokenKeyword.h"
#include "Tokens/TokenOperator.h"
#include "Tokens/TokenString.h"
#include "Tokens/TokenBaseIdentifier.h"
#include "Tokens/TokenGlobalIdentifier.h"
#include "Tokens/TokenLocalIdentifier.h"
#include "Tokens/TokenLocalUnnamedIdentifier.h"
#include "Tokens/TokenLocalIdentifier.h"
#include "Tokens/TokenParenthesis.h"
#include "Tokens/TokenCurlyBrace.h"
#include "Tokens/TokenTypeInteger.h"
#include "Tokens/TokenNumberLiteral.h"

#include "Types/TypeInteger.h"

#include "Expressions/ExpressionSourceFile.h"
#include "Expressions/ExpressionDataLayout.h"
#include "Expressions/ExpressionFunctionHeaderPreName.h"
#include "Expressions/ExpressionLocalNamedIdentifier.h"
#include "Expressions/ExpressionLocalUnnamedIdentifier.h"

#include "Expressions/Instructions/InstructionAlloca.h"

namespace Parser {
    template <typename T>
    bool Parser::checkReserved(int pos, EnumRegistry::RegistryItem reserved) {
        auto token = (*tokens)[pos];
        return ((typeid(*token) == typeid(T))
                && (std::dynamic_pointer_cast<const T>(token)->registryItem == reserved));
    }
    
    template<typename T>
    std::string Parser::extractNamedIdentifier(int pos) {
        static_assert(std::is_base_of<Token::TokenBaseIdentifier, T>(), "Type <T> must be a derivative of TokenBaseIdentifier");
        tokenPointer token = (*tokens)[pos];
        if (typeid(*token) == typeid(T)) {
            return std::dynamic_pointer_cast<const T>(token)->identifier;
        } else {
            throw ParsingException("Received incorrect identifier type at source position "+std::to_string(token->srcPos)+".", pos);
        }
    }

    stringExtractResult Parser::attemptExtractString(int pos) {
        auto token = (*tokens)[pos];
        bool isString = (typeid(*token) == typeid(Token::TokenString));
        return isString ? stringExtractResult(std::dynamic_pointer_cast<const Token::TokenString>(token)->str, true)
                        : stringExtractResult("", false);
    }

    std::string Parser::extractString(int pos) {
        tokenPointer token = getToken(pos);
        if (isType<Token::TokenString>(token)) {
            return std::dynamic_pointer_cast<const Token::TokenString>(token)->str;
        } else {
            throw ParsingException("Expected TokenString at token position "+std::to_string(token->srcPos)+", instead received: "+token->getName(), pos);
        }
    }

    int Parser::extractInteger(int pos) {
        tokenPointer token = getToken(pos);
        if (isType<Token::TokenNumberLiteral>(token)) {
            return std::dynamic_pointer_cast<const Token::TokenNumberLiteral>(token)->val;
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

    Instructions::InstructionParseResult Parser::parseInstructionAlloca(int startPos) {
        int currPos = startPos;
        bool success = false;
        Lib::ResultPointer<Expressions::ExpressionIdentifier> identifierResult = parseIdentifier(currPos);
        std::shared_ptr<const Types::TypeSized> allocationType;
        bool inalloca = false;
        int alignment = -1;
        if (identifierResult.success) {
            currPos = identifierResult.newPos;
            if (checkReserved<Token::TokenOperator>(currPos, Operators::equals)) {
                currPos++;
                if (checkReserved<Token::TokenKeyword>(currPos, ReservedWords::alloca)) {
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
                        if (checkReserved<Token::TokenKeyword>(currPos, ReservedWords::align)) {
                            currPos++;
                            alignment = extractInteger(currPos);
                            currPos++;
                        }

                        // Try parsing the address space section.
                        // TODO
                        success = true;
                    }
                }
            }
        }
        return success ? Instructions::InstructionParseResult(std::make_shared<Instructions::InstructionAlloca>
                                                                (identifierResult.result, inalloca, allocationType, alignment), currPos)
                        : Instructions::InstructionParseResult();
    }

    /** GENERAL PARSE METHODS **/

    Lib::ResultPointer<Expressions::ExpressionIdentifier> Parser::parseIdentifier(int startPos) {
        tokenPointer token = getToken(startPos);
        int nextPos = startPos+1;
        bool success = false;
        std::shared_ptr<const Expressions::ExpressionIdentifier> identifier;
        // FIXME Implement proper hierarchy for identifiers
        if (isType<Token::TokenLocalUnnamedIdentifier>(token)) {
            identifier = std::make_shared<const Expressions::ExpressionLocalUnnamedIdentifier>(std::dynamic_pointer_cast<const Token::TokenLocalUnnamedIdentifier>(token)->ID);
            success = true;
        } else if (isType<Token::TokenLocalIdentifier>(token)) {
            identifier = std::make_shared<const Expressions::ExpressionLocalNamedIdentifier>(std::dynamic_pointer_cast<const Token::TokenLocalIdentifier>(token)->identifier);
            success = true;
        }
        return success ? Lib::ResultPointer<Expressions::ExpressionIdentifier>(identifier, nextPos)
                        : Lib::ResultPointer<Expressions::ExpressionIdentifier>();
    }

    Lib::ResultPointer<Types::Type> Parser::parseType(int startPos) {
        int nextPosition=-1;
        std::shared_ptr<const Token::Token> firstToken = (*tokens)[startPos];
        std::shared_ptr<const Types::Type> determinedType;
        bool success = false;
        if (typeid(firstToken) == typeid(Token::TokenTypeInteger)) {
            determinedType = std::make_shared<const Types::TypeInteger>(std::dynamic_pointer_cast<const Token::TokenTypeInteger>(firstToken)->bitWidth);
            success = true;
            nextPosition = startPos+1;
        }
        return success ? Lib::ResultPointer<Types::Type>(determinedType, nextPosition) : Lib::ResultPointer<Types::Type>();
    }

    Lib::ResultPointer<Types::TypeSized> Parser::parseSizedType(int startPos) {
        int nextPosition=-1;
        std::shared_ptr<const Token::Token> firstToken = (*tokens)[startPos];
        std::shared_ptr<const Types::TypeSized> determinedType;
        bool success = false;
        if (typeid(firstToken) == typeid(Token::TokenTypeInteger)) {
            determinedType = std::make_shared<const Types::TypeInteger>(std::dynamic_pointer_cast<const Token::TokenTypeInteger>(firstToken)->bitWidth);
            success = true;
            nextPosition = startPos+1;
        }
        return success ? Lib::ResultPointer<Types::TypeSized>(determinedType, nextPosition) : Lib::ResultPointer<Types::TypeSized>();
    }

    ParsingResult<Expressions::ExpressionSourceFile> Parser::parseSourceFile(int startPos) {
        
        int currPos = startPos;
        int nextTokenAfterExpr = currPos;
        std::string str = "";
        bool success = false;
        if (checkReserved<Token::TokenKeyword>(currPos, ReservedWords::source_filename)) {
            currPos++;
            if (checkReserved<Token::TokenOperator>(currPos, Operators::equals)) {
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
        if (checkReserved<Token::TokenKeyword>(currPos, ReservedWords::target)) {
            currPos++;
            if (checkReserved<Token::TokenKeyword>(currPos, ReservedWords::datalayout)) {
                currPos++;
                if (checkReserved<Token::TokenOperator>(currPos, Operators::equals)) {
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
        if (checkReserved<Token::TokenKeyword>(currPos, ReservedWords::target)) {
            currPos++;
            if (checkReserved<Token::TokenKeyword>(currPos, ReservedWords::triple)) {
                currPos++;
                if (checkReserved<Token::TokenOperator>(currPos, Operators::equals)) {
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
        
        std::shared_ptr<std::vector<std::shared_ptr<const Token::TokenKeyword>>> keywords = std::make_shared<std::vector<std::shared_ptr<const Token::TokenKeyword>>>();
        int currPos = startPos;
        // Parse linkages:

        // Parse Runtime Preemption Specifiers - need to add dso_preemptable
        if (checkReserved<Token::TokenKeyword>(currPos, ReservedWords::dso_local)) {
            keywords->push_back(std::dynamic_pointer_cast<const Token::TokenKeyword>((*tokens)[currPos]));
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
        std::shared_ptr<std::vector<std::shared_ptr<const Token::TokenKeyword>>> attributes
            = std::make_shared<std::vector<std::shared_ptr<const Token::TokenKeyword>>>();
        if (checkReserved<Token::TokenKeyword>(currPos, ReservedWords::noundef)) {
            attributes->push_back(std::dynamic_pointer_cast<const Token::TokenKeyword>((*tokens)[currPos]));
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
        bool success = false;
        int currPos = startPos;
        if (isLeftToken<Token::TokenParenthesis>(currPos)) {
            currPos++;
            // TODO Capture arguments
            if (isRightToken<Token::TokenParenthesis>(currPos)) {
                currPos++;
                success = true;
            }
        }
        return success ? ParsingResult<Expressions::ExpressionArgumentList>(std::make_shared<Expressions::ExpressionArgumentList>(), currPos)
                        : ParsingResult<Expressions::ExpressionArgumentList>();
    }

    ParsingResult<Expressions::ExpressionFunctionHeaderPostName> Parser::parseFunctionHeaderPostName(int startPos) {
        std::shared_ptr<std::vector<std::shared_ptr<const Token::TokenKeyword>>> keywords = std::make_shared<std::vector<std::shared_ptr<const Token::TokenKeyword>>>();
        std::shared_ptr<std::map<std::string, std::string>> strKeywords = std::make_shared<std::map<std::string, std::string>>();
        int currPos = startPos;
        // Parse unamed_addr:

        // Parse address space:

        // Parse function attributes:
        bool parseFunctionAttribute = true;
        while (parseFunctionAttribute && currPos < tokens->size()) {
            tokenPointer token = (*tokens)[currPos];
            if (typeid(*token) == typeid(Token::TokenKeyword)) {
                std::shared_ptr<const Token::TokenKeyword> keywordToken = std::dynamic_pointer_cast<const Token::TokenKeyword>(token);
                EnumRegistry::RegistryItem keyword = keywordToken->registryItem;
                if (keyword == ReservedWords::mustprogress || keyword == ReservedWords::noinline || keyword == ReservedWords::norecurse
                    || keyword == ReservedWords::nounwind || keyword == ReservedWords::optnone || keyword == ReservedWords::uwtable) {
                        keywords->push_back(keywordToken);
                        currPos++;
                }
            } else {
                stringExtractResult strLabelResult = attemptExtractString(currPos);
                if (strLabelResult.second) { // Should this verify that the string value is an accepted keyword?
                    if (checkReserved<Token::TokenOperator>(currPos+1, Operators::equals)) {
                        std::string strValue = extractString(currPos+2);
                        (*strKeywords)[strLabelResult.first] = strValue;
                        currPos += 3;
                    } // What if this fails?
                } else {
                    std::cout << token->getNameAndPos() << '\n';
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

    CodeBlockParsingResult Parser::parseFunctionCodeBlock(int startPos, int startUnnamedLocal, std::shared_ptr<std::set<std::string>> localNameSet) {
        int nextUnnamedLocal = startUnnamedLocal;
        int currPos = startPos;
        std::string label;
        auto newLocalNameSet = std::make_shared<std::set<std::string>>();
        std::copy(newLocalNameSet->begin(), newLocalNameSet->end(), std::inserter(*localNameSet, localNameSet->begin()));
        
        if (false) {

        } else { // No label
            label = std::to_string(nextUnnamedLocal);
            nextUnnamedLocal++;
        }
        
        // Parse code
        bool foundTerminator = false;
        bool parseInstructions = true;
        do {
            auto tokenPointer = (*tokens)[currPos];
            stringExtractResult result = stringExtractResult("", false);
            if (typeid(tokenPointer) == typeid(Token::TokenLocalUnnamedIdentifier)) {
                int id = std::dynamic_pointer_cast<const Token::TokenLocalUnnamedIdentifier>(tokenPointer)->ID;
                if (id != nextUnnamedLocal) {
                    throw ParsingException("LLVM IR is a single-assignment language. Duplicate assignment at source position "
                                            +std::to_string(tokenPointer->srcPos)+".", currPos);
                }
                nextUnnamedLocal++;
                currPos++;
                result = stringExtractResult(std::to_string(id), true);
                localNameSet->insert(result.first);
            } else if (typeid(tokenPointer) == typeid(Token::TokenLocalIdentifier)) {
                std::string identifier = std::dynamic_pointer_cast<const Token::TokenLocalIdentifier>(tokenPointer)->identifier;
                if (localNameSet->count(identifier)) {
                    throw ParsingException("LLVM IR is a single-assignment language. Duplicate assignment at source position "
                                            +std::to_string(tokenPointer->srcPos)+".", currPos);
                }
                currPos++;
                result = stringExtractResult(identifier, true);
                localNameSet->insert(identifier);
            }
            if (result.second) { // Instruction that returns a value.
                if (checkReserved<Token::TokenOperator>(currPos , Operators::equals)) {

                }
            } else {

            }

        } while (parseInstructions);
        // Parse terminator

    }

    ParsingResult<std::vector<std::shared_ptr<Expressions::ExpressionFunctionCodeBlock>>> Parser::parseFunctionCodeBlocks(int startPos) {
        int currPos = startPos;
        bool success = false;
        auto codeBlocks = std::make_shared<std::vector<std::shared_ptr<Expressions::ExpressionFunctionCodeBlock>>>();
        auto localNameSet = std::make_shared<std::set<std::string>>();
        if (isLeftToken<Token::TokenCurlyBrace>(currPos)) { // Function code section should open with a '{'
            currPos++;
            int currUnnamedLocal = 0;
            bool parseBlocks = true;
            bool notEmpty = false;
            do {
                auto parseResult = parseFunctionCodeBlock(currPos, currUnnamedLocal, localNameSet);
                if (parseResult.success) {
                    codeBlocks->push_back(parseResult.expression);
                    currPos = parseResult.newPos;
                    currUnnamedLocal = parseResult.nextUnnamedLocal;
                    localNameSet = parseResult.localNameSet;
                    notEmpty = true;
                } else {
                    parseBlocks = false;
                }
            } while (parseBlocks);
            if (notEmpty) { // Must contain at least one code block.
                if (isRightToken<Token::TokenCurlyBrace>(currPos)) { // Function code section closes with a '}'
                    currPos++;
                    success = true;
                }
            }
        }
        return success ? ParsingResult<std::vector<std::shared_ptr<Expressions::ExpressionFunctionCodeBlock>>>(codeBlocks, currPos)
                        : ParsingResult<std::vector<std::shared_ptr<Expressions::ExpressionFunctionCodeBlock>>>();
    }

    ParsingResult<Expressions::ExpressionFunctionDefinition> Parser::parseFunctionDefinition(int startPos) {
        bool success = false;
        int currPos = startPos;
        if (checkReserved<Token::TokenKeyword>(currPos, ReservedWords::define)) {
            currPos++;
            auto headerPreName = parseFunctionHeaderPreName(currPos);
            if (headerPreName.success) {
                currPos = headerPreName.newPos;
                auto returnType = parseFunctionReturnType(currPos);
                if (returnType.success) {
                    currPos = returnType.newPos;
                    std::string functionName = extractNamedIdentifier<Token::TokenGlobalIdentifier>(currPos);
                    currPos++;
                    auto argumentsList = parseFunctionArgumentList(currPos);
                    if (argumentsList.success) {
                        currPos = argumentsList.newPos;
                        auto headerPostName = parseFunctionHeaderPostName(currPos);
                        if (headerPostName.success) {
                            std::cout << std::to_string(headerPostName.expression->keywords->size()) << ' ' << std::to_string(headerPostName.expression->strKeywords->size()) << '\n';
                        }
                    }
                }
            }
        }
        throw ParsingException("parseFunctionDefinition not fully implemented.", startPos);
    }

    std::shared_ptr<const Expressions::ExpressionFile> Parser::parse() {
        
        bool success = false;
        int pos = 0;
        auto srcResult = parseSourceFile(pos);
        if (!srcResult.success) {
            throw ParsingException("Missing source filename at start of file.", pos);
        }
        // The datalayout and target triples are optional.
        //  Should I have their parse methods throw ParsingExceptions?
        //  Maybe the different parse...(...) methods should throw ParsingExceptions if they can't successfully parse their intended
        //  expression, and the function that calls them is responsible for determining whether or not to ignore them?
        // Are these required to be at the start of the file?
        pos = srcResult.newPos;
        auto dataLayoutResult = parseDataLayout(pos);
        if (dataLayoutResult.success) pos = dataLayoutResult.newPos;
        auto targetTripleResult = parseTargetTriple(pos);
        if (targetTripleResult.success) pos = targetTripleResult.newPos;
        auto functionResult = parseFunctionDefinition(pos);
        
        std::shared_ptr<const Expressions::ExpressionFile> result
                    = std::make_shared<Expressions::ExpressionFile>(std::dynamic_pointer_cast<const Expressions::ExpressionSourceFile>(srcResult.expression),
                                                                    std::dynamic_pointer_cast<const Expressions::ExpressionDataLayout>(dataLayoutResult.expression));
        
        return result;
    }
}