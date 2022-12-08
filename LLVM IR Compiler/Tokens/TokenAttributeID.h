#ifndef LLVM_IR_COMPILER_TOKENATTRIBUTEID_H
#define LLVM_IR_COMPILER_TOKENATTRIBUTEID_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Tokens {
    class TokenAttributeID : public Token {
        public:
            const int attributeID;
            TokenAttributeID(int attributeIDArg, int srcPosArg) : attributeID(attributeIDArg), Token(srcPosArg) {}
            std::string getName() const {return "TokenAttributeID("+std::to_string(attributeID)+")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenAttributeIDConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENATTRIBUTEID_H
