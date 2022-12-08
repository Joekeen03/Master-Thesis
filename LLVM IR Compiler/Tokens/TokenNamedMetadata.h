#ifndef LLVM_IR_COMPILER_TOKENNamedMetadata_H
#define LLVM_IR_COMPILER_TOKENNamedMetadata_H

#include <memory>
#include <iostream>

#include "TokenMetadataIdentifier.h"

namespace Tokens {
    class TokenNamedMetadata : public TokenMetadataIdentifier {
        public:
            const std::string name;
            TokenNamedMetadata(std::string nameArg, int srcPosArg) : name(nameArg), TokenMetadataIdentifier(srcPosArg) {}
            std::string getName() const {return "TokenNamedMetadata("+name+")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenNamedMetadataConstructor : public TokenMetadataIdentifierConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENNamedMetadata_H
