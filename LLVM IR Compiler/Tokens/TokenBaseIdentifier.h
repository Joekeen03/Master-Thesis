#ifndef LLVM_IR_COMPILER_TOKENBASEIDENTIFIER_H
#define LLVM_IR_COMPILER_TOKENBASEIDENTIFIER_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Token {
    class TokenBaseIdentifier : public Token::Token {
        public:
            const std::string identifier;
            TokenBaseIdentifier(std::string identifierArg, int srcPosArg) : identifier(identifierArg), Token(srcPosArg) {}
            std::string getName() const { return "Generic Identifier Token"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenBaseIdentifierConstructor : public TokenConstructor { // TODO: Should this be a singleton?
    };
}

#endif // LLVM_IR_COMPILER_TOKENBASEIDENTIFIER_H
