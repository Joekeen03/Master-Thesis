#ifndef LLVM_IR_COMPILER_TOKENGlobalNamedIdentifier_H
#define LLVM_IR_COMPILER_TOKENGlobalNamedIdentifier_H

#include <memory>
#include <iostream>

#include "TokenGlobalIdentifier.h"

namespace Tokens {
    class TokenGlobalNamedIdentifier : public TokenGlobalIdentifier {
        public:
            const std::string name;
            explicit TokenGlobalNamedIdentifier(std::string nameArg, int srcPosArg) : name(nameArg), TokenGlobalIdentifier(srcPosArg) {}
            std::string getName() const {return "TokenGlobalNamedIdentifier"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenGlobalNamedIdentifierConstructor : public TokenGlobalIdentifierConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENGlobalNamedIdentifier_H
