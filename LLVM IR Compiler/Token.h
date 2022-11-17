#ifndef LLVM_IR_COMPILER_TOKEN_H
#define LLVM_IR_COMPILER_TOKEN_H

#include <memory>

#include "BasicCharArray.h"

namespace Token {
    class Token {
        // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
        //  To perhaps optimize how the tokenizer iterates through them.
    };

    struct TokenizeResult {
        public:
            TokenizeResult() : token(), newPos(-1), success(false) {}
            TokenizeResult(Token t, int p) : token(t), newPos(p), success(true) {}
            const Token token;
            const int newPos;
            const bool success;
            TokenizeResult& operator=(TokenizeResult& other) { // FIXME Does this work?
                return other;
            }

    };

    class TokenConstructor { // TODO: Should this be a singleton?
        public:
            virtual TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int currPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKEN_H