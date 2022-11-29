#ifndef LLVM_IR_COMPILER_TOKENCurlyBrace_H
#define LLVM_IR_COMPILER_TOKENCurlyBrace_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Token {
    class TokenCurlyBrace : public Token::Token {
        public:
            const bool leftBrace;
            TokenCurlyBrace(bool leftBraceArg, int srcPosArg) : leftBrace(leftBraceArg), Token(srcPosArg) {}
            std::string getName() const {
                std::string paren = leftBrace ? "{" : "}";
                return "TokenCurlyBrace(\""+ paren +"\")";
            }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenCurlyBraceConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENCurlyBrace_H
