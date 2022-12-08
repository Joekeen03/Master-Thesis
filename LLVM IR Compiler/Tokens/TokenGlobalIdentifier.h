#ifndef LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H
#define LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H

#include <string>
#include <string>

#include "TokenIdentifier.h"

namespace Tokens {
    class TokenGlobalIdentifier : public TokenIdentifier {
        public:
            TokenGlobalIdentifier(int srcPosArg) : TokenIdentifier(srcPosArg) {}
            std::string getName() const {return "TokenGlobalIdentifier"; }
    };

    class TokenGlobalIdentifierConstructor : public TokenIdentifierConstructor {
    };
}
#endif // LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H