#ifndef LLVM_IR_COMPILER_TOKENNamedGlobalIdentifier_H
#define LLVM_IR_COMPILER_TOKENNamedGlobalIdentifier_H

#include <memory>
#include <iostream>

#include "TokenGlobalIdentifier.h"

namespace Tokens {
    class TokenNamedGlobalIdentifier : public TokenGlobalIdentifier {
        public:
            explicit TokenNamedGlobalIdentifier(int srcPosArg) : TokenGlobalIdentifier(srcPosArg) {}
            std::string getName() const {return "TokenNamedGlobalIdentifier"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenNamedGlobalIdentifierConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENNamedGlobalIdentifier_H
