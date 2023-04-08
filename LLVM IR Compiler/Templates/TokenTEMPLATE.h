#ifndef LLVM_IR_COMPILER_TOKENTEMPLATE_H
#define LLVM_IR_COMPILER_TOKENTEMPLATE_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Tokens {
    class TokenTEMPLATE : public Token {
        public:
            explicit TokenTEMPLATE(int srcPosArg) : Token(srcPosArg) {}
            std::string getName() const override {return "TokenTEMPLATE"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenTEMPLATEConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos) override;
    };
}

#endif // LLVM_IR_COMPILER_TOKENTEMPLATE_H