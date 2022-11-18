#ifndef LLVM_IR_COMPILER_TOKENKEYWORD_H
#define LLVM_IR_COMPILER_TOKENKEYWORD_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Token {
    class TokenKeyword : public Token::Token {
        public:
            const int keywordID;
            TokenKeyword(int keywordIDArg) : keywordID(keywordIDArg) {}
            std::string getName() const {return "Keyword Token"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenKeywordConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENKEYWORD_H