#ifndef LLVM_IR_COMPILER_TOKEN_H
#define LLVM_IR_COMPILER_TOKEN_H

#include <memory>
#include <iostream>

#include "../BasicCharArray.h" // FIXME Properly set up include path

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

    };

    class TokenConstructor { // TODO: Should this be a singleton?
        public:
            virtual TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
                std::cout << "Default tokenize method called.\n";
                return TokenizeResult();
            }
    };
}

#endif // LLVM_IR_COMPILER_TOKEN_H