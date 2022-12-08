#ifndef LLVM_IR_COMPILER_TOKENLocalIdentifier_H
#define LLVM_IR_COMPILER_TOKENLocalIdentifier_H

#include <memory>
#include <iostream>

#include "TokenIdentifier.h"

namespace Tokens {
    class TokenLocalIdentifier : public TokenIdentifier {
        public:
            TokenLocalIdentifier(int srcPosArg) : TokenIdentifier(srcPosArg) {}
            std::string getName() const {return "TokenLocalIdentifier"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenLocalIdentifierConstructor : public TokenIdentifierConstructor { // TODO: Should this be a singleton?
    };
}

#endif // LLVM_IR_COMPILER_TOKENLocalIdentifier_H
