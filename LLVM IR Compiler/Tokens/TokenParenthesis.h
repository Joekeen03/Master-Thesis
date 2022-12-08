#ifndef LLVM_IR_COMPILER_TOKENPARENTHESIS_H
#define LLVM_IR_COMPILER_TOKENPARENTHESIS_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Tokens {
    class TokenParenthesis : public Token {
        public:
            const bool left;
            TokenParenthesis(bool leftArg, int srcPosArg) : left(leftArg), Token(srcPosArg) {}
            std::string getName() const {
                std::string paren = left ? "(" : ")";
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