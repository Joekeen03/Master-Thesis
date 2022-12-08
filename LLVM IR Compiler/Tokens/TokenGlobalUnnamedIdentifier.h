#ifndef LLVM_IR_COMPILER_TOKENGlobalUnnamedIdentifier_H
#define LLVM_IR_COMPILER_TOKENGlobalUnnamedIdentifier_H

#include <memory>
#include <iostream>

#include "TokenGlobalIdentifier.h"

namespace Tokens {
    class TokenGlobalUnnamedIdentifier : public TokenGlobalIdentifier {
        public:
            const int ID;
            TokenGlobalUnnamedIdentifier(int IDArg, int srcPosArg) : ID(IDArg), TokenGlobalIdentifier(srcPosArg) {}
            std::string getName() const {return "TokenGlobalUnnamedIdentifier("+std::to_string(ID)+")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenGlobalUnnamedIdentifierConstructor : public TokenGlobalIdentifierConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENGlobalUnnamedIdentifier_H
