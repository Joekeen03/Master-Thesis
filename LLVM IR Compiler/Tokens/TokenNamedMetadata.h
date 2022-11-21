#ifndef LLVM_IR_COMPILER_TOKENNamedMetadata_H
#define LLVM_IR_COMPILER_TOKENNamedMetadata_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Token {
    class TokenNamedMetadata : public Token::Token {
        public:
            const std::string name;
            TokenNamedMetadata(std::string nameArg) : name(nameArg) {}
            std::string getName() const {return "NamedMetadata Token"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenNamedMetadataConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENNamedMetadata_H
