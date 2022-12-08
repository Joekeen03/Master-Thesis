#ifndef LLVM_IR_COMPILER_TOKENNumberLiteral_H
#define LLVM_IR_COMPILER_TOKENNumberLiteral_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Tokens {
    class TokenNumberLiteral : public Token {
        public:
            const int val;
            TokenNumberLiteral(int valArg, int srcPosArg) : val(valArg), Token(srcPosArg) {}
            std::string getName() const {return "TokenNumberLiteral("+std::to_string(val)+")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenNumberLiteralConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENNumberLiteral_H
