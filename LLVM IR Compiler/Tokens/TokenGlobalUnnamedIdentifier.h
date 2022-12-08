#ifndef LLVM_IR_COMPILER_TOKENGlobalUnnamedIdentifier_H
#define LLVM_IR_COMPILER_TOKENGlobalUnnamedIdentifier_H

#include <memory>
#include <iostream>

#include "TokenBaseUnnamedIdentifier.h"

namespace Tokens {
    class TokenGlobalUnnamedIdentifier : public TokenBaseUnnamedIdentifier {
        public:
            TokenGlobalUnnamedIdentifier(int IDArg, int srcPosArg) : TokenBaseUnnamedIdentifier(IDArg, srcPosArg) {}
            std::string getName() const {return "TokenGlobalUnnamedIdentifier("+std::to_string(ID)+")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenGlobalUnnamedIdentifierConstructor : public TokenBaseUnnamedIdentifierConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENGlobalUnnamedIdentifier_H
