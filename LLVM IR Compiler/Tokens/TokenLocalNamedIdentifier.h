#ifndef LLVM_IR_COMPILER_TOKENLocalNamedIdentifier_H
#define LLVM_IR_COMPILER_TOKENLocalNamedIdentifier_H

#include <memory>
#include <iostream>

#include "TokenLocalIdentifier.h"

namespace Tokens {
    class TokenLocalNamedIdentifier : public TokenLocalIdentifier {
        public:
            const std::string name;
            explicit TokenLocalNamedIdentifier(std::string nameArg, int srcPosArg) : name(nameArg), TokenLocalIdentifier(srcPosArg) {}
            std::string getName() const {return "TokenLocalNamedIdentifier"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenLocalNamedIdentifierConstructor : public TokenLocalIdentifierConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENLocalNamedIdentifier_H
