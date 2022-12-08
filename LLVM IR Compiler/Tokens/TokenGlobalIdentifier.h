#ifndef LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H
#define LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H

#include <string>
#include <string>

#include "TokenBaseIdentifier.h"

namespace Tokens {
    class TokenGlobalIdentifier : public TokenBaseIdentifier {
        public:
            TokenGlobalIdentifier(std::string identifierArg, int srcPosArg) : TokenBaseIdentifier(identifierArg, srcPosArg) {}
            std::string getName() const {return "TokenGlobalIdentifier("+identifier+")"; }
    };

    class TokenGlobalIdentifierConstructor : public TokenBaseIdentifierConstructor {
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}
#endif // LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H