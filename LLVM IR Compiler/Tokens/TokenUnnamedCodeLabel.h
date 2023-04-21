#ifndef LLVM_IR_COMPILER_TOKENUnnamedCodeLabel_H
#define LLVM_IR_COMPILER_TOKENUnnamedCodeLabel_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Tokens {
    class TokenUnnamedCodeLabel : public Token {
        public:
            const unsigned int ID;
            explicit TokenUnnamedCodeLabel(int srcPosArg, unsigned int IDArg) : Token(srcPosArg), ID(IDArg) {}
            std::string getName() const override {return "TokenUnnamedCodeLabel("+std::to_string(ID)+")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenUnnamedCodeLabelConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos) override;
    };
}

#endif // LLVM_IR_COMPILER_TOKENUnnamedCodeLabel_H
