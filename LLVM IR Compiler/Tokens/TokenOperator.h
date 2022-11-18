#ifndef LLVM_IR_COMPILER_TOKENOPERATOR_H
#define LLVM_IR_COMPILER_TOKENOPERATOR_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Token {
    class TokenOperator : public Token::Token {
        public:
            const int operandID;
            TokenOperator(int operandIDArg) : operandID(operandIDArg) {}
            std::string getName() const {return "Operator Token"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenOperatorConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENOPERATOR_H