#ifndef LLVM_IR_COMPILER_TOKENLocalIdentifier_H
#define LLVM_IR_COMPILER_TOKENLocalIdentifier_H

#include <memory>
#include <iostream>

#include "TokenBaseIdentifier.h"

namespace Token {
    class TokenLocalIdentifier : public TokenBaseIdentifier {
        public:
            TokenLocalIdentifier(std::string identifierArg, int srcPosArg) : TokenBaseIdentifier(identifierArg, srcPosArg) {}
            std::string getName() const {return "TokenLocalIdentifier("+identifier+")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenLocalIdentifierConstructor : public TokenBaseIdentifierConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENLocalIdentifier_H
