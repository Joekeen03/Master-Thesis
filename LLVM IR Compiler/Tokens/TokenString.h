#ifndef LLVM_IR_COMPILER_TOKENSTRING_H
#define LLVM_IR_COMPILER_TOKENSTRING_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Token {
    class TokenString : public Token::Token {
        public:
            const std::string str;
            TokenString(std::string strArg) : str(strArg) {}
            std::string getName() const {return "String Token"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenStringConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENSTRING_H