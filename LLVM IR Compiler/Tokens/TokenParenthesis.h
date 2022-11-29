#ifndef LLVM_IR_COMPILER_TOKENPARENTHESIS_H
#define LLVM_IR_COMPILER_TOKENPARENTHESIS_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Token {
    class TokenParenthesis : public Token::Token {
        public:
            const bool leftParen;
            TokenParenthesis(bool leftParenArg, int srcPosArg) : leftParen(leftParenArg), Token(srcPosArg) {}
            std::string getName() const {
                std::string paren = leftParen ? "(" : ")";
                return "TokenParenthesis(\""+ paren +"\")";
            }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenParenthesisConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENPARENTHESIS_H