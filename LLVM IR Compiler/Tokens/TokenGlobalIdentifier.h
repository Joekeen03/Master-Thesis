#ifndef LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H
#define LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H

#include <string>
#include <string>

#include "Token.h"

namespace Token {
    class TokenGlobalIdentifier : public Token::Token {
        public:
            std::string identifier;
            TokenGlobalIdentifier(std::string identifierArg) : identifier(identifierArg) {}
            std::string getName() const {return "Global Identifier Token"; }
    };

    class TokenGlobalIdentifierConstructor : public TokenConstructor {
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}
#endif // LLVM_IR_COMPILER_TOKENGLOBALIDENTIFIER_H