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
#include "Tokens/TokenUnnamedCodeLabel.h"
#include "Tokens/TokenEOF.h"

#include "Types/TypeInteger.h"

#include "Expressions/ExpressionSourceFile.h"
#include "Expressions/ExpressionDataLayout.h"
#include "Expressions/ExpressionFunctionHeaderPreName.h"
#include "Expressions/ExpressionLocalNamedIdentifier.h"
#include "Expressions/ExpressionLocalUnnamedIdentifier.h"
#include "Expressions/ExpressionValueInteger.h"
#include "Expressions/ExpressionValueMetadataString.h"

#include "Expressions/Instructions/InstructionAlloca.h"
#include "Expressions/Instructions/InstructionStore.h"
#include "Expressions/Instructions/InstructionRetValue.h"
#include "Expressions/Instructions/YieldsValue.h"

namespace Parser {
    template <typename T>
    T Parser::updateError(int newTokenPosition, std::string message) {
        if (error == nullptr || newTokenPosition >= error->tokenPosition) {
            error = std::make_shared<ParseError>(newTokenPosition, message);
        }
        return T();
    }

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

    InstructionParseResult Parser::parseInstructionStore(int startPos) {
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
        auto instruction = Instructions::InstructionStore(valueType, valueOperand, pointerOperand, alignment);
        return success ? InstructionParseResult(std::make_shared<Instructions::InstructionVariant>(instruction), currPos)
                        : InstructionParseResult();
    }

    // Instructions that yield values

    InstructionParseResult Parser::parseInstructionAlloca(int startPos, std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assignee) {
        int currPos = startPos;
        bool success = false;
        std::shared_ptr<const Types::TypeSized> allocationType;
        bool inalloca = false;
        int alignment = -1;
        if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::alloca)) {
            currPos++;
            // Try parsing an 'inalloca' token
            // TODO

            Lib::ResultConstMembersPointer<Types::TypeSized> typeResult = parseSizedType(currPos);
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
        auto instruction = Instructions::InstructionAlloca(assignee, inalloca, allocationType, alignment);
        return success ? InstructionParseResult(std::make_shared<Instructions::InstructionVariant>(instruction), currPos)
                        : InstructionParseResult();
    }

    InstructionParseResult Parser::parseInstructionLoad(int startPos, std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assignee) {
        int currPos = startPos;
        if (!checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::load)) {
            return InstructionParseResult();
        }

        // Is there some way to reduce how repetitive this is?
        currPos++;
        auto typeResult = parseSizedType(currPos);
        if (!typeResult.success) {
            return InstructionParseResult();
        }

        currPos = typeResult.newPos;
        if (!Lib::isType<Tokens::TokenComma>(getToken(currPos))) {
            return updateErrorExpectedReceived<InstructionParseResult>(currPos, "load instruction", "comma");
        }

        currPos++;
        if (!checkReserved<Tokens::TokenTypeReservedWord>(currPos, TypeReservedWords::typePtr)) {
            return updateErrorExpectedReceived<InstructionParseResult>(currPos, "load instruction", "ptr");
        }

        currPos++;
        auto pointerIdentifierResult = parseIdentifier(currPos);
        if (!pointerIdentifierResult.success) {
            return InstructionParseResult();
        }
        
        currPos = pointerIdentifierResult.newPos;
        int alignment = -1;
        if (Lib::isType<Tokens::TokenComma>(getToken(currPos)) && checkReserved<Tokens::TokenKeyword>(currPos+1, ReservedWords::align)) {
            currPos += 2;
            alignment = extractInteger(currPos);
            currPos++;
        }
        
        auto loadInstruction = Instructions::InstructionLoad(assignee, typeResult.result, pointerIdentifierResult.result, alignment);
        return InstructionParseResult(std::make_shared<Instructions::InstructionVariant>(loadInstruction), currPos);
    } // parseInstructionLoad

    InstructionParseResult Parser::parseInstructionAdd(int startPos, std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assignee) {
        int currPos = startPos;
        if (!checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::add)) {
            return updateErrorExpectedReceived<InstructionParseResult>(currPos, "add instruction", "add");
        }

        currPos++;
        bool nuw = false;
        bool nsw = false;
        if (checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::nsw)) {
            nsw = true;
            currPos++;
        }

        std::shared_ptr<const Types::TypeSized> yieldedType;
        if (auto integerParseResult = parseIntegerType(currPos); integerParseResult.success) {
            yieldedType = integerParseResult.result;
            currPos = integerParseResult.newPos;
        } else { // FIXME add integer vectors - maybe move this stuff out into a parseIntegerOrIntegerVector(...) method.
            // Maybe should just be return InstructionParseResult();
            return InstructionParseResult();
        }

        auto leftOperandResult = parseOperand(currPos);
        if (!leftOperandResult.success) {
            return InstructionParseResult();
        }
        currPos = leftOperandResult.newPos;
        
        if (!isTokenOfType<Tokens::TokenComma>(currPos)) {
            return updateErrorExpectedReceived<InstructionParseResult>(currPos, "add instruction", "comma");
        }
        currPos++;

        auto rightOperandResult = parseOperand(currPos);
        if (!rightOperandResult.success) {
            return InstructionParseResult();
        }
        currPos = rightOperandResult.newPos;
        auto instructionAdd = Instructions::InstructionAdd(assignee, nsw, nuw, yieldedType,
                                                            leftOperandResult.result, rightOperandResult.result);
        return InstructionParseResult(std::make_shared<Instructions::InstructionVariant>(instructionAdd), currPos);
    } // parseInstructionAdd

    InstructionParseResult Parser::parseInstructionTruncate(int startPos, std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assignee) {
        // FIXME should validate the final type's size is smaller than the initial type's size.
        int currPos = startPos;
        if (!checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::trunc)) {
            return updateErrorExpectedReceived<InstructionParseResult>(currPos, "truncate instruction", "truncate keyword");
        }
        
        currPos++;
        auto operandTypeParseResult = parseIntegerType(currPos);
        if (!operandTypeParseResult.success) {
            return InstructionParseResult();
        }

        currPos = operandTypeParseResult.newPos;
        auto operandParseResult = parseOperand(currPos);
        if (!operandParseResult.success) {
            return InstructionParseResult();
        }
        
        currPos = operandParseResult.newPos;
        if (!checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::to)) {
            return updateErrorExpectedReceived<InstructionParseResult>(currPos, "truncate instruction", "'to' keyword");
        }

        currPos++;
        auto yieldedTypeParseResult = parseIntegerType(currPos);
        if (!yieldedTypeParseResult.success) {
            return InstructionParseResult();
        }

        currPos = yieldedTypeParseResult.newPos;
        auto instruction = Instructions::InstructionTruncate(assignee, operandTypeParseResult.result,
                                                            operandParseResult.result, yieldedTypeParseResult.result);
        return InstructionParseResult(std::make_shared<const Instructions::InstructionVariant>(instruction), currPos);
    } // parseInstructionTruncate

    // Terminator Instructions

    InstructionParseResult Parser::parseInstructionReturnValue(int startPos) {
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
        auto instruction = Instructions::InstructionRetValue(retType, retVal);
        return success ? InstructionParseResult(std::make_shared<Instructions::InstructionVariant>(instruction), nextPos)
                        : InstructionParseResult();
    } // parseInstructionReturnValue

    InstructionParseResult Parser::parseInstructionBranchConditional(int startPos) {
        int currPos = startPos;
        if (!checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::br)) {
            return updateErrorExpectedReceived<InstructionParseResult>(currPos, "conditional branch instruction", "'br' keyword");
        }

        currPos++;
        if (!parse_i1BooleanType(currPos).success) {
            return updateErrorExpectedReceived<InstructionParseResult>(currPos, "conditional branch instruction", "i1 integer type");
        }

        currPos++;
        auto conditionParseResult = parseLocalIdentifier(currPos);
        if (!conditionParseResult.success) {
            return InstructionParseResult();
        }

        currPos = conditionParseResult.newPos;
        if (!isTokenOfType<Tokens::TokenComma>(currPos)) {
            return updateErrorExpectedReceived<InstructionParseResult>(currPos, "conditional branch instruction", "comma");
        }

        currPos++;
        auto ifTrueLabelParseResult = parseLocalIdentifierAsLabel(currPos);
        if (!ifTrueLabelParseResult.success) {
            return InstructionParseResult();
        }

        currPos = ifTrueLabelParseResult.newPos;
        if (!isTokenOfType<Tokens::TokenComma>(currPos)) {
            return updateErrorExpectedReceived<InstructionParseResult>(currPos, "conditional branch instruction", "comma");
        }

        currPos++;
        auto ifFalseLabelParseResult = parseLocalIdentifierAsLabel(currPos);
        if (!ifFalseLabelParseResult.success) {
            return InstructionParseResult();
        }

        currPos = ifFalseLabelParseResult.newPos;
        auto instruction = Instructions::InstructionBranchConditional(conditionParseResult.result, ifTrueLabelParseResult.result,
                                                                      ifFalseLabelParseResult.result);
        return InstructionParseResult(std::make_shared<Instructions::InstructionVariant>(instruction), currPos);
    } // parseInstructionBranchConditional

    InstructionParseResult Parser::parseInstructionBranchUnconditional(int startPos) {
        int currPos = startPos;
        if (!checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::br)) {
            return updateErrorExpectedReceived<InstructionParseResult>(currPos, "unconditional branch instruction", "'br' keyword");
        }

        currPos++;
        auto destinationLabel = parseLocalIdentifierAsLabel(currPos);
        if (!destinationLabel.success) {
            return InstructionParseResult();
        }

        currPos = destinationLabel.newPos;
        auto instruction = Instructions::InstructionBranchUnconditional(destinationLabel.result);
        return InstructionParseResult(std::make_shared<Instructions::InstructionVariant>(instruction), currPos);
    } // parseInstructionBranchUnconditional



    /** GENERAL PARSE METHODS **/

    Lib::ResultConstMembersPointer<Expressions::ExpressionIdentifier> Parser::parseIdentifier(int startPos) {
        bool success = false;
        std::shared_ptr<const Expressions::ExpressionIdentifier> identifier;
        int nextPos;
        auto localIdentifierResult = parseLocalIdentifier(startPos);
        if (localIdentifierResult.success) {
            identifier = localIdentifierResult.result;
            nextPos = localIdentifierResult.newPos;
            success = true;
        }
        return success ? Lib::ResultConstMembersPointer<Expressions::ExpressionIdentifier>(identifier, nextPos)
                        : Lib::ResultConstMembersPointer<Expressions::ExpressionIdentifier>();
    } // parseIdentifier

    Lib::ResultConstMembersPointer<Expressions::ExpressionLocalIdentifier> Parser::parseLocalIdentifier(int startPos) {
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
        return success ? Lib::ResultConstMembersPointer<Expressions::ExpressionLocalIdentifier>(identifier, nextPos)
                        : Lib::ResultConstMembersPointer<Expressions::ExpressionLocalIdentifier>();
    } // parseLocalIdentifier

    ParsingResult<Expressions::ExpressionLocalIdentifier> Parser::parseLocalIdentifierAsLabel(int startPos) {
        int currPos = startPos;
        if (!checkReserved<Tokens::TokenTypeReservedWord>(currPos, TypeReservedWords::typeLabel)) {
            return updateErrorExpectedReceived<ParsingResult<Expressions::ExpressionLocalIdentifier>>(currPos, "local identifier as code block label", "label type");
        }

        currPos++;
        auto labelParseResult = parseLocalIdentifier(currPos);
        if (!labelParseResult.success) {
            return updateErrorExpectedReceived<ParsingResult<Expressions::ExpressionLocalIdentifier>>(currPos, "local identifier as code block label", "local identifier");
        }

        return labelParseResult;
    }



    ParsingResult<Types::TypeInteger> Parser::parseIntegerType(int startPos) {
        int currPos = startPos;
        auto firstToken = getToken(startPos);
        if (!Lib::isType<Tokens::TokenTypeInteger>(firstToken)) {
            return updateErrorExpectedReceived<ParsingResult<Types::TypeInteger>>(startPos, "integer type", "integer type");
        }
        currPos++;
        auto integerType = std::make_shared<const Types::TypeInteger>(std::dynamic_pointer_cast<const Tokens::TokenTypeInteger>(firstToken)->bitWidth);
        return Lib::ResultConstMembersPointer<Types::TypeInteger>(integerType, startPos+1);
    } // parseIntegerType

    ParsingResult<Types::TypeInteger> Parser::parse_i1BooleanType(int startPos) {
        auto integerTypeParseResult = parseIntegerType(startPos);
        if (integerTypeParseResult.success && integerTypeParseResult.result->bits == 1) {
            return integerTypeParseResult;
        }
        return updateErrorExpectedReceived<ParsingResult<Types::TypeInteger>>(startPos, "boolean", "i1 integer type");
    } // parse_i1BooleanType

    Lib::ResultConstMembersPointer<Types::Type> Parser::parseType(int startPos) {
        if (auto integerTypeParseResult = parseIntegerType(startPos); integerTypeParseResult.success) {
            return Lib::ResultConstMembersPointer<Types::Type>(integerTypeParseResult.result, integerTypeParseResult.newPos);
        }
        return Lib::ResultConstMembersPointer<Types::Type>();
    }

    Lib::ResultConstMembersPointer<Types::TypeSized> Parser::parseSizedType(int startPos) {
        if (auto integerTypeParseResult = parseIntegerType(startPos); integerTypeParseResult.success) {
            return Lib::ResultConstMembersPointer<Types::TypeSized>(integerTypeParseResult.result, integerTypeParseResult.newPos);
        }
        return Lib::ResultConstMembersPointer<Types::TypeSized>();
    }

    Lib::ResultConstMembersPointer<Types::TypeSized> Parser::parseFirstClassKnownSizeType(int startPos) {
        if (auto integerTypeParseResult = parseIntegerType(startPos); integerTypeParseResult.success) {
            return Lib::ResultConstMembersPointer<Types::TypeSized>(integerTypeParseResult.result, integerTypeParseResult.newPos);
        }
        return Lib::ResultConstMembersPointer<Types::TypeSized>();
    }

    Lib::ResultConstMembersPointer<Expressions::ExpressionOperand> Parser::parseOperand(int startPos) {
        int currPos = startPos;
        int nextPos = -1;
        // Attempt to parse a literal
        auto numberResult = attemptExtractInteger(currPos);
        if (numberResult.second) {
            auto operandValue = std::make_shared<const Expressions::operandVariant>(Expressions::ExpressionLiteralInteger(numberResult.first));
            // auto x = Expressions::ExpressionOperand(operandValue);
            auto operandExpression = std::make_shared<const Expressions::ExpressionOperand>(operandValue);
            nextPos = currPos+1;
            return Lib::ResultConstMembersPointer<Expressions::ExpressionOperand>(operandExpression, nextPos);
        }
        // Attempt to parse an identifier.
        auto identifierResult = parseIdentifier(currPos);
        if (identifierResult.success) {
            std::shared_ptr<const Expressions::operandVariant> operandValue;
            // FIXME change parseIdentifier's result to contain a pointer to a variant of different identifiers, rather
            //  than a pointer to a generic identifier.
            if (Lib::isType<Expressions::ExpressionLocalNamedIdentifier>(identifierResult.result)) {
                auto castIdentifier = std::dynamic_pointer_cast<const Expressions::ExpressionLocalNamedIdentifier>(identifierResult.result);
                operandValue = std::make_shared<const Expressions::operandVariant>(*castIdentifier);
            } else if (Lib::isType<Expressions::ExpressionLocalUnnamedIdentifier>(identifierResult.result)) {
                auto castIdentifier = std::dynamic_pointer_cast<const Expressions::ExpressionLocalUnnamedIdentifier>(identifierResult.result);
                operandValue = std::make_shared<const Expressions::operandVariant>(*castIdentifier);
            } else {
                throw std::runtime_error("Unhandled identifier type in Parser::parseOperand.");
            }
            auto operandExpression = std::make_shared<const Expressions::ExpressionOperand>(operandValue);
            nextPos = identifierResult.newPos;
            return Lib::ResultConstMembersPointer<Expressions::ExpressionOperand>(operandExpression, nextPos);
        }
        
        return updateError<Lib::ResultConstMembersPointer<Expressions::ExpressionOperand>>(currPos, "Error parsing operand: expected a literal or identifier, received a "+getToken(currPos)->getName());
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

    ParsingResult<Expressions::ExpressionArgumentList> Parser::parseFunctionArgumentList(int startPos, std::shared_ptr<LocalsParsingInfo> localsInfo) {
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

    InstructionParseResult Parser::parseInstruction(int startPos) {
        auto localIdentifierResult = parseLocalIdentifier(startPos);
        int currPos = startPos;
        if (localIdentifierResult.success) {
            currPos = localIdentifierResult.newPos;
            if (!checkReserved<Tokens::TokenOperator>(currPos, Operators::equals)) {
                return updateError<InstructionParseResult>(currPos, expectedReceivedMessage("Yields-value instruction", "equals", currPos));
            }
            currPos++;
            for (int i = 0; i < nInstructionsYieldValue; i++)
            {
                auto valueInstructionParser = instructionsYieldValue[i];
                auto result = (this->*valueInstructionParser)(currPos, localIdentifierResult.result);
                if (result.success) return result;
            }
        } else {
            for (int i = 0; i < nInstructionsYieldVoid; i++)
            {
                auto voidInstructionParser = instructionsYieldVoid[i];
                auto result = (this->*voidInstructionParser)(currPos);
                if (result.success) return result;
            }
        }
        return updateError<InstructionParseResult>(currPos, expectedReceivedMessage("instruction", "instruction keyword", currPos));
    }

    ParsingResult<Expressions::ExpressionFunctionCodeBlock> Parser::parseFunctionCodeBlock(int startPos, std::shared_ptr<LocalsParsingInfo> localsInfo) {
        // FIXME: move stuff regarding identifier validation, out of this (environment object that manages that, or even a second
        //  pass)
        int currPos = startPos;
        std::string label;
        auto instructions = std::make_shared<std::vector<const std::shared_ptr<const Instructions::NonTerminators>>>();
        std::shared_ptr<const Instructions::Terminators> terminator;
        
        
        if (auto nextToken = getToken(currPos); Lib::isType<const Tokens::TokenUnnamedCodeLabel>(nextToken)) {
            auto castToken = std::dynamic_pointer_cast<const Tokens::TokenUnnamedCodeLabel>(nextToken);
            int ID = castToken->ID;
            if (localsInfo->validateAndAddNewUnnamedLocalIdentifier(ID)) {
                label = std::to_string(ID);
                currPos++;
            } else {
                std::string error = "Error parsing code block: Encountered code block with an unnamed label,"\
                                    " but label was not the next one in the unnamed locals identifiers sequence. Expected: "
                                    +std::to_string(localsInfo->peekNewUnnamedLocalIdentifier())
                                    +"; received: "+std::to_string(ID)+".";
                return updateError<ParsingResult<Expressions::ExpressionFunctionCodeBlock>>(currPos, error);
            }
        } else { // No label
            label = std::to_string(localsInfo->getNewUnnamedLocalIdentifier());
        }
        
        // Parse code
        bool parseInstructions = true;
        bool foundTerminator = false;
        
        do {
            auto instructionResult = parseInstruction(currPos);
            if (instructionResult.success) {
                // Validate assignee, if any.
                const std::optional<int> invalidAssignee = std::visit([&localsInfo](auto&& instruction) -> std::optional<int> {
                    if constexpr (std::is_base_of_v<Instructions::YieldsValue, std::remove_reference_t<decltype(instruction)>>) {
                        const std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assignee = instruction.assignee;
                        if (Lib::isType<const Expressions::ExpressionLocalUnnamedIdentifier>(assignee)) {
                            auto castAssignee = std::dynamic_pointer_cast<const Expressions::ExpressionLocalUnnamedIdentifier>(assignee);
                            int ID = castAssignee->ID;
                            bool isValidAssignee = localsInfo->validateAndAddNewUnnamedLocalIdentifier(ID);
                            if (!isValidAssignee) {
                                return ID;
                            }
                        } else {
                            throw std::runtime_error("NYI: validation for local named identifiers.");
                        }
                    }
                    return std::optional<int>();
                }, *instructionResult.result);
                if (invalidAssignee) {
                    std::string error = "Error parsing code block: Encountered instruction which assignes to an unnamed local,"\
                                        " but label was not the next one in the integer locals identifiers sequence. Expected: "
                                        +std::to_string(localsInfo->peekNewUnnamedLocalIdentifier())
                                        +"; received: "+std::to_string(invalidAssignee.value())+".";
                    return updateError<ParsingResult<Expressions::ExpressionFunctionCodeBlock>>(currPos, error);
                }

                // FIXME Move into separate method
                // if (Lib::isType<Instructions::YieldsValue>(instructionResult.result)) {
                //     // Ensure the local assigned to isn't a duplicate assignment (violating SSA), and if it's an unnamed local,
                //     //  that it matches with the next unnamed local ID (local ID definitions must be monotonically increasing, as
                //     //      in %0=, %1=, %2=, %3=, ...)
                //     auto assignedIdentifier = std::dynamic_pointer_cast<const Instructions::YieldsValue>(instructionResult.result)->assignee;
                //     if (Lib::isType<Expressions::ExpressionLocalUnnamedIdentifier>(assignedIdentifier)) {
                //         auto identifier = std::dynamic_pointer_cast<const Expressions::ExpressionLocalUnnamedIdentifier>(assignedIdentifier);
                //         if (identifier->ID < nextUnnamedLocal) {
                //             throw ParsingException("LLVM IR is a single-assignment language. Duplicate unnamed local assignment at source position "
                //                                     +std::to_string(getToken(currPos)->srcPos)+".", currPos);
                //         } else if (identifier->ID > nextUnnamedLocal) {
                //             throw ParsingException("Unnamed locals must be defined consecutively - 1, 2, 3...; local definition at sourcePosition"
                //                                     +std::to_string(getToken(currPos)->srcPos)+" was not the expected ID, "
                //                                     +std::to_string(nextUnnamedLocal)+".", currPos);
                //         }
                //         newLocalNameSet->insert(std::to_string(identifier->ID));
                //     } else if (Lib::isType<Expressions::ExpressionLocalNamedIdentifier>(assignedIdentifier)) {
                //         auto identifier = std::dynamic_pointer_cast<const Expressions::ExpressionLocalNamedIdentifier>(assignedIdentifier);
                //         if (newLocalNameSet->count(identifier->name)) {
                //             throw ParsingException("LLVM IR is a single-assignment language. Duplicate named local assignment at source position "
                //                                     +std::to_string(getToken(currPos)->srcPos)+".", currPos);
                //         }
                //         newLocalNameSet->insert(identifier->name);
                //     }
                // }
                
                // Which of this is more idiomatic? This, which captures more local variables, or the alternative (below),
                //  which checks the instruction's type twice and has to have a dummy else clause that returns
                //  std::monostate()?
                // This is far more succint, so that would seem to be preferable for now.
                // std:: visit(overloaded {})
                std::visit([&terminator, &parseInstructions, &foundTerminator, instructions](auto&& instruction) mutable {
                    if constexpr (std::is_base_of_v<Instructions::InstructionTerminator, std::remove_reference_t<decltype(instruction)>>) {
                        terminator = std::make_shared<Instructions::Terminators>(instruction);
                        parseInstructions = false;
                        foundTerminator = true;
                    } else { // Should explicitly check for InstructionNonTerminator?
                        instructions->push_back(std::make_shared<Instructions::NonTerminators>(instruction));
                    }
                }, *(instructionResult.result));


                // Alternative:

                // if (std::visit([](auto&& arg) -> bool {return Lib::isBaseOf<Instructions::InstructionTerminator>(arg);}, *(instructionResult.result))) {
                //     terminator = std::make_shared<Instructions::Terminators>(std::visit([](auto&& arg) -> Instructions::Terminators {
                //             typedef decltype(arg) T;
                //             if constexpr (std::is_base_of<Instructions::InstructionTerminator, T>::value) {
                //                 return arg;
                //             } else {
                //                 return std::monostate();
                //             }
                //         }, *(instructionResult.result)));
                    
                //     // terminator = std::make_shared<Instructions::Terminators>(instructionResult.result);
                //     parseInstructions = false;
                //     foundTerminator = true;
                // } else {
                //     instructions->push_back(std::make_shared<Instructions::NonTerminators>(instructionResult.result));
                // }
                currPos = instructionResult.newPos;
            } else {
                parseInstructions = false;
            }

        } while (parseInstructions);
        
        return foundTerminator ? ParsingResult<Expressions::ExpressionFunctionCodeBlock>(std::make_shared<Expressions::ExpressionFunctionCodeBlock>(label, instructions, terminator), currPos)
                                : ParsingResult<Expressions::ExpressionFunctionCodeBlock>();
    }

    Lib::ResultConstMembersPointer<std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock>>> Parser::parseFunctionCodeBlocks(int startPos, std::shared_ptr<LocalsParsingInfo> localsInfo) {
        int currPos = startPos;
        bool success = false;
        auto codeBlocks = std::make_shared<std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock>>>();
        if (isLeftToken<Tokens::TokenCurlyBrace>(currPos)) { // Function code section should open with a '{'
            currPos++;
            bool parseBlocks = true;
            bool notEmpty = false;
            do { // Build list of code blocks
                auto parseResult = parseFunctionCodeBlock(currPos, localsInfo);
                if (parseResult.success) {
                    codeBlocks->push_back(parseResult.result);
                    currPos = parseResult.newPos;
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
        return success ? Lib::ResultConstMembersPointer<std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock>>>(codeBlocks, currPos)
                        : Lib::ResultConstMembersPointer<std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock>>>();
    }

    Lib::ResultConstMembersPointer<Expressions::ExpressionFunctionDefinition> Parser::parseFunctionDefinition(int startPos) {
        using resultType = Lib::ResultConstMembersPointer<Expressions::ExpressionFunctionDefinition>;
        int currPos = startPos;
        int nextPos = -1;
        auto localsInfo = std::make_shared<LocalsParsingInfo>();

        std::shared_ptr<const Expressions::ExpressionFunctionHeaderPreName> headerPreName;
        std::shared_ptr<const Expressions::ExpressionReturnType> returnType;
        std::string functionName;
        std::shared_ptr<const Expressions::ExpressionArgumentList> parameterList;
        std::shared_ptr<const Expressions::ExpressionFunctionHeaderPostName> headerPostName;
        std::shared_ptr<const std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock>>> codeBlocks;

        if (!checkReserved<Tokens::TokenKeyword>(currPos, ReservedWords::define)) {
            return updateError<resultType>(currPos, expectedReceivedMessage("function definition", "define", currPos));
        }

        currPos++;
        auto headerPreNameResult = parseFunctionHeaderPreName(currPos);
        if (!headerPreNameResult.success) {
            return resultType();
        }

        headerPreName = headerPreNameResult.result;
        currPos = headerPreNameResult.newPos;
        auto returnTypeResult = parseFunctionReturnType(currPos);
        if (!returnTypeResult.success) {
            return resultType();
        }

        returnType = returnTypeResult.result;
        currPos = returnTypeResult.newPos;
        // Do functions have to use named identifiers for their name?
        functionName = extractNamedIdentifier<Tokens::TokenGlobalNamedIdentifier>(currPos);
        currPos++;
        auto parameterListResult = parseFunctionArgumentList(currPos, localsInfo);
        if (!parameterListResult.success) {
            return resultType();
        }
        
        parameterList = parameterListResult.result;
        currPos = parameterListResult.newPos;
        auto headerPostNameResult = parseFunctionHeaderPostName(currPos);
        if (!headerPostNameResult.success) {
            return resultType();
        }
        
        headerPostName = headerPostNameResult.result;
        currPos = headerPostNameResult.newPos;
        auto codeBlocksResult = parseFunctionCodeBlocks(currPos, localsInfo);
        if (!codeBlocksResult.success) {
            return resultType();
        }
        codeBlocks = codeBlocksResult.result;
        nextPos = codeBlocksResult.newPos;
        
        std::cout << "Parsed function definition." << '\n';
        auto functionDefinition = std::make_shared<Expressions::ExpressionFunctionDefinition>(headerPreName, returnType, functionName, parameterList, headerPostName, codeBlocks);
        return Lib::ResultConstMembersPointer<Expressions::ExpressionFunctionDefinition>(functionDefinition, nextPos);
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
                updateError<ParsingResult<int>>(pos, "Unrecognized module-level structure start token: "+token->getNameAndPos()+".");
                outputToken(pos);
                outputToken(pos+1);
                outputToken(pos+2);
                if (error != nullptr) {
                    // FIXME make tokenizer hold source line & position w/in line, instead of/in addition to character position.
                    throw ParsingException("Furthest token position reached: "+std::to_string(error->tokenPosition)
                                            +" (source position: "+std::to_string(getToken(error->tokenPosition)->srcPos)+")"
                                            +".\nError: "+error->message, error->tokenPosition);
                } else {
                    throw ParsingException("Unrecognized module-level structure start token: "+token->getNameAndPos()+".", pos);
                }
            }
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
        &Parser::parseInstructionReturnValue,
        &Parser::parseInstructionBranchConditional,
        &Parser::parseInstructionBranchUnconditional,

        // Normal Instructions:
        &Parser::parseInstructionStore,
    };
    const int nInstructionsYieldVoid = sizeof(instructionsYieldVoid)/sizeof(instructionYieldsVoidParser);
    const instructionYieldsValueParser instructionsYieldValue[] = {
        &Parser::parseInstructionAlloca,
        &Parser::parseInstructionLoad,
        &Parser::parseInstructionAdd,
        &Parser::parseInstructionTruncate,
    };
    const int nInstructionsYieldValue = sizeof(instructionsYieldValue)/sizeof(instructionYieldsValueParser);
}