#include "Parser.h"

#include <typeinfo>

#include "Tokens/TokenKeyword.h"
#include "Tokens/TokenOperator.h"
#include "Tokens/TokenString.h"

#include "ParseExpressions/ExpressionSourceFile.h"
#include "ParseExpressions/ExpressionDataLayout.h"

namespace Parser {
    template <typename T>
    bool Parser::checkReserved(tokenPointer token, EnumRegistry::RegistryItem reserved) {
        return ((typeid(*token) == typeid(T))
                && (std::dynamic_pointer_cast<const T>(token)->registryItem == reserved));
    }

    stringExtractResult Parser::attemptExtractString(tokenPointer token) {
        bool isString = (typeid(*token) == typeid(Token::TokenString));
        return isString ? stringExtractResult(std::dynamic_pointer_cast<const Token::TokenString>(token)->str, true)
                        : stringExtractResult("", false);
    }

    std::string Parser::extractString(tokensArrayPointer tokens, int pos) {
        if (typeid(*(*tokens)[pos]) == typeid(Token::TokenString)) {
            return std::dynamic_pointer_cast<const Token::TokenString>((*tokens)[pos])->str;
        } else {
            throw ParsingException("Expected TokenString at token position "+std::to_string(pos)+", instead received: "+(*tokens)[pos]->getName());
        }
    }

    ParsingResult Parser::parseSourceFile(tokensArrayPointer tokens, int startPos) {
        int currPos = startPos;
        int nextTokenAfterExpr = currPos;
        std::string str = "";
        bool success = false;
        if (checkReserved<Token::TokenKeyword>((*tokens)[currPos], ReservedWords::source_filename)) {
            currPos++;
            if (checkReserved<Token::TokenOperator>((*tokens)[currPos], Operators::equals)) {
                currPos++;
                str = extractString(tokens, currPos);
                success = true;
                nextTokenAfterExpr = currPos+1;
            }
        }
        return success ? ParsingResult(std::make_shared<Expression::ExpressionSourceFile>(str), nextTokenAfterExpr): ParsingResult();
    }

    ParsingResult Parser::parseDataLayout(tokensArrayPointer tokens, int startPos) {
        int currPos = startPos;
        int nextTokenAfterExpr = currPos;
        std::string str = "";
        bool success = false;
        if (checkReserved<Token::TokenKeyword>((*tokens)[currPos], ReservedWords::target)) {
            currPos++;
            if (checkReserved<Token::TokenKeyword>((*tokens)[currPos], ReservedWords::datalayout)) {
                currPos++;
                if (checkReserved<Token::TokenOperator>((*tokens)[currPos], Operators::equals)) {
                    currPos++;
                    // FIXME Was it better to use the attemptExtractString(...) instead?
                    str = extractString(tokens, currPos);
                    success = true;
                    nextTokenAfterExpr = currPos+1;
                }
            }
            
        }
        return success ? ParsingResult(std::make_shared<Expression::ExpressionDataLayout>(str), nextTokenAfterExpr): ParsingResult();
    }

    ParsingResult Parser::parseTargetTriple(tokensArrayPointer tokens, int startPos) {
        int currPos = startPos;
        int nextTokenAfterExpr = currPos;
        std::string str = "";
        bool success = false;
        // FIXME Should it throw a ParsingException if any of the other if (...) statements fail?
        if (checkReserved<Token::TokenKeyword>((*tokens)[currPos], ReservedWords::target)) {
            currPos++;
            if (checkReserved<Token::TokenKeyword>((*tokens)[currPos], ReservedWords::triple)) {
                currPos++;
                if (checkReserved<Token::TokenOperator>((*tokens)[currPos], Operators::equals)) {
                    currPos++;
                    str = extractString(tokens, currPos);
                    success = true;
                    nextTokenAfterExpr = currPos+1;
                }
            }
            
        }
        return success ? ParsingResult(std::make_shared<Expression::ExpressionDataLayout>(str), nextTokenAfterExpr): ParsingResult();
    }

    std::shared_ptr<const Expression::ExpressionFile> Parser::parse(tokensArrayPointer tokens) {
        bool success = false;
        ParsingResult srcResult = parseSourceFile(tokens, 0);
        if (!srcResult.success) {
            throw ParsingException("Missing source filename at start of file.");
        }
        // The datalayout and target triples are optional.
        //  Should I have their parse methods throw ParsingExceptions?
        //  Maybe the different parse...(...) methods should throw ParsingExceptions if they can't successfully parse their intended
        //  expression, and the function that calls them is responsible for determining whether or not to ignore them?
        // Are these required to be at the start of the file?
        ParsingResult dataLayoutResult = parseDataLayout(tokens, srcResult.newPos);
        ParsingResult targetTripleResult = parseTargetTriple(tokens, dataLayoutResult.newPos);
        std::shared_ptr<const Expression::ExpressionFile> result
                    = std::make_shared<Expression::ExpressionFile>(std::dynamic_pointer_cast<const Expression::ExpressionSourceFile>(srcResult.expression),
                                                                    std::dynamic_pointer_cast<const Expression::ExpressionDataLayout>(dataLayoutResult.expression));
        
        return result;
    }
}