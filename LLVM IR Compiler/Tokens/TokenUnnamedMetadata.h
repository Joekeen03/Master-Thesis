#ifndef LLVM_IR_COMPILER_TOKENUnnamedMetadata_H
#define LLVM_IR_COMPILER_TOKENUnnamedMetadata_H

#include <memory>
#include <iostream>

#include "TokenMetadataIdentifier.h"
#include "HelperFunctionsTokens.h"

namespace Tokens {
    class TokenUnnamedMetadata : public TokenMetadataIdentifier {
        public:
            const int ID;
            explicit TokenUnnamedMetadata(int IDArg, int srcPosArg) : ID(IDArg), TokenMetadataIdentifier(srcPosArg) {}
            std::string getName() const {return "TokenUnnamedMetadata"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenUnnamedMetadataConstructor : public TokenMetadataIdentifierConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENUnnamedMetadata_H
