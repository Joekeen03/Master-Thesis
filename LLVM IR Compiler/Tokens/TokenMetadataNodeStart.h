#ifndef LLVM_IR_COMPILER_TOKENMetadataNodeStart_H
#define LLVM_IR_COMPILER_TOKENMetadataNodeStart_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Token {
    class TokenMetadataNodeStart : public Token::Token {
        public:
            TokenMetadataNodeStart() {}
            std::string getName() const {return "MetadataNodeStart Token"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenMetadataNodeStartConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENMetadataNodeStart_H
