#ifndef LLVM_IR_COMPILER_TOKENLocalUnnamedIdentifier_H
#define LLVM_IR_COMPILER_TOKENLocalUnnamedIdentifier_H

#include <memory>
#include <iostream>

#include "TokenLocalIdentifier.h"

namespace Tokens {
    class TokenLocalUnnamedIdentifier : public TokenLocalIdentifier {
        public:
            const int ID;
            TokenLocalUnnamedIdentifier(int IDArg, int srcPosArg) : ID(IDArg), TokenLocalIdentifier(srcPosArg) {}
            std::string getName() const {return "TokenLocalUnnamedIdentifier("+std::to_string(ID)+")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenLocalUnnamedIdentifierConstructor : public TokenLocalIdentifierConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENLocalUnnamedIdentifier_H
