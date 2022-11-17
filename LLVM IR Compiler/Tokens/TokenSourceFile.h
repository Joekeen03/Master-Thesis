#ifndef LLVM_IR_COMPILER_TOKENSOURCEFILE_H
#define LLVM_IR_COMPILER_TOKENSOURCEFILE_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Token {
    class TokenSourceFile : public Token::Token {
        // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
        //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenSourceFileConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENSOURCEFILE_H