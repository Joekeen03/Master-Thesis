#ifndef LLVM_IR_COMPILER_TOKENType_H
#define LLVM_IR_COMPILER_TOKENType_H

#include <memory>
#include <iostream>

#include "Token.h"
#include "../Types/Type.h"
#include "../EnumRegistry.h"

namespace Tokens {
    class TokenType : public Token {
        public:
            explicit TokenType(int srcPosArg) : Token(srcPosArg) {}
            std::string getName() const {return "TokenType"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenTypeConstructor : public TokenConstructor { // TODO: Should this be a singleton?
    };
}

#endif // LLVM_IR_COMPILER_TOKENType_H
