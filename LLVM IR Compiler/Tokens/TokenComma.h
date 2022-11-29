#ifndef LLVM_IR_COMPILER_TOKENComma_H
#define LLVM_IR_COMPILER_TOKENComma_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Token {
    class TokenComma : public Token::Token {
        public:
            explicit TokenComma(int srcPosArg) : Token(srcPosArg) {}
            std::string getName() const {return "TokenComma"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenCommaConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENComma_H
