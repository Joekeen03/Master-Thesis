#include "Parser.h"

#include <typeinfo>

#include "Tokens/TokenKeyword.h"
#include "Tokens/TokenOperator.h"
#include "Tokens/TokenString.h"

#include "ParseExpressions/ExpressionSourceFile.h"
#include "ParseExpressions/ExpressionDataLayout.h"

namespace Parser {
    bool Parser::checkKeyword(std::shared_ptr<const Token::Token> token, EnumRegistry::RegistryItem keyword) {
        return ((typeid(*token) == typeid(Token::TokenKeyword))
                && (std::dynamic_pointer_cast<const Token::TokenKeyword>(token)->registryItem == keyword));
    }

    bool Parser::checkOperand(std::shared_ptr<const Token::Token> token, EnumRegistry::RegistryItem op) {
        return ((typeid(*token) == typeid(Token::TokenOperator))
                && (std::dynamic_pointer_cast<const Token::TokenOperator>(token)->registryItem == op));
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
        if (checkKeyword((*tokens)[currPos], ReservedWords::source_filename)) {
            currPos++;
            if (checkOperand((*tokens)[currPos], Operators::equals)) {
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
        if (checkKeyword((*tokens)[currPos], ReservedWords::target)) {
            currPos++;
            if (checkKeyword((*tokens)[currPos], ReservedWords::datalayout)) {
                currPos++;
                if (checkOperand((*tokens)[currPos], Operators::equals)) {
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
}