#ifndef LLVM_IR_COMPILER_TOKENIdentifier_H
#define LLVM_IR_COMPILER_TOKENIdentifier_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Tokens {
    class TokenIdentifier : public Token {
        public:
            explicit TokenIdentifier(int srcPosArg) : Token(srcPosArg) {}
            std::string getName() const {return "TokenIdentifier"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenIdentifierConstructor : public TokenConstructor { // TODO: Should this be a singleton?
    };
}

#endif // LLVM_IR_COMPILER_TOKENIdentifier_H
