#ifndef LLVM_IR_COMPILER_TOKENEOF_H
#define LLVM_IR_COMPILER_TOKENEOF_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Tokens {
    class TokenEOF : public Token {
        public:
            explicit TokenEOF(int srcPosArg) : Token(srcPosArg) {}
            std::string getName() const { return "TokenEOF"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenEOFConstructor { // TODO: Should this be a singleton?
        public:
            std::shared_ptr<TokenEOF> createEOFToken(BasicArray::BasicCharArray* fileData, int finalPos) const;
    };
}

#endif // LLVM_IR_COMPILER_TOKENEOF_H
