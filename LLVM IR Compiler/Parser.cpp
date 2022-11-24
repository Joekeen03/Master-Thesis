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

    ParsingResult Parser::parseSourceFile(tokensArrayPointer tokens, int startPos) {
        int currPos = startPos;
        int nextTokenAfterExpr = currPos;
        std::string str = "";
        bool success = false;
        if (checkReserved<Token::TokenKeyword>((*tokens)[currPos], ReservedWords::source_filename)) {
            currPos++;
            if (checkReserved<Token::TokenOperator>((*tokens)[currPos], Operators::equals)) {
                currPos++;
                stringExtractResult result = attemptExtractString((*tokens)[currPos]);
                if (result.second) {
                    str = result.first;
                    success = true;
                    nextTokenAfterExpr = currPos+1;
                }
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
                    stringExtractResult result = attemptExtractString((*tokens)[currPos]);
                    if (result.second) {
                        str = result.first;
                        success = true;
                        nextTokenAfterExpr = currPos+1;
                    }
                }
            }
            
        }
        return success ? ParsingResult(std::make_shared<Expression::ExpressionDataLayout>(str), nextTokenAfterExpr): ParsingResult();
    }

    std::shared_ptr<const Expression::ExpressionFile> Parser::parse(tokensArrayPointer tokens) {
        std::shared_ptr<const Expression::ExpressionFile> result;
        bool success = false;
        ParsingResult srcResult = parseSourceFile(tokens, 0);
        if (srcResult.success) {
            ParsingResult dataLayoutResult = parseDataLayout(tokens, srcResult.newPos);
            if (dataLayoutResult.success) {
                result = std::make_shared<Expression::ExpressionFile>(std::dynamic_pointer_cast<const Expression::ExpressionSourceFile>(srcResult.expression),
                                                    std::dynamic_pointer_cast<const Expression::ExpressionDataLayout>(dataLayoutResult.expression));
                success = true;
            }
        }
        if (success) return result;
        throw ParsingException("Couldn't parse the provided tokens.");
    }
}