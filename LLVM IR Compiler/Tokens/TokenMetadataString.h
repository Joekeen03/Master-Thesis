#ifndef LLVM_IR_COMPILER_TOKENMetadataString_H
#define LLVM_IR_COMPILER_TOKENMetadataString_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Tokens {
    class TokenMetadataString : public Token {
        public:
            const std::string str;
            TokenMetadataString(std::string strArg, int srcPosArg) : str(strArg), Token(srcPosArg) {}
            std::string getName() const {return "TokenMetadataString("+str+")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenMetadataStringConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENMetadataString_H
