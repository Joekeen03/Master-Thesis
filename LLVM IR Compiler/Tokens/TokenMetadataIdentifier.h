#ifndef LLVM_IR_COMPILER_TOKENMetadataIdentifier_H
#define LLVM_IR_COMPILER_TOKENMetadataIdentifier_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Tokens {
    class TokenMetadataIdentifier : public Token {
        public:
            explicit TokenMetadataIdentifier(int srcPosArg) : Token(srcPosArg) {}
            std::string getName() const {return "TokenMetadataIdentifier"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenMetadataIdentifierConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENMetadataIdentifier_H
