#ifndef LLVM_IR_COMPILER_TOKENNamedLocalIdentifier_H
#define LLVM_IR_COMPILER_TOKENNamedLocalIdentifier_H

#include <memory>
#include <iostream>

#include "TokenLocalIdentifier.h"

namespace Tokens {
    class TokenNamedLocalIdentifier : public TokenLocalIdentifier {
        public:
            explicit TokenNamedLocalIdentifier(int srcPosArg) : TokenLocalIdentifier(srcPosArg) {}
            std::string getName() const {return "TokenNamedLocalIdentifier"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenNamedLocalIdentifierConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENNamedLocalIdentifier_H
