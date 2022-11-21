#ifndef LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H
#define LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H

#include <string>
#include <string>

#include "TokenBaseIdentifier.h"

namespace Token {
    class TokenGlobalIdentifier : public TokenBaseIdentifier {
        public:
            TokenGlobalIdentifier(std::string identifierArg) : TokenBaseIdentifier(identifierArg) {}
            std::string getName() const {return "TokenGlobalIdentifier("+identifier+")"; }
    };

    class TokenGlobalIdentifierConstructor : public TokenBaseIdentifierConstructor {
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}
#endif // LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H