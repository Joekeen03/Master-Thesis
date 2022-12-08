#ifndef LLVM_IR_COMPILER_TOKENTypeInteger_H
#define LLVM_IR_COMPILER_TOKENTypeInteger_H

#include <memory>
#include <iostream>

#include "TokenType.h"

namespace Tokens {
    class TokenTypeInteger : public TokenType {
        public:
            const int bitWidth;
            explicit TokenTypeInteger(int bitWidthArg, int srcPosArg) : bitWidth(bitWidthArg), TokenType(srcPosArg) {}
            std::string getName() const {return "TokenTypeInteger("+std::to_string(bitWidth)+")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenTypeIntegerConstructor : public TokenTypeConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENTypeInteger_H
