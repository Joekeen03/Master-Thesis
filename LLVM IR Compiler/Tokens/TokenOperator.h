#ifndef LLVM_IR_COMPILER_TOKENOPERATOR_H
#define LLVM_IR_COMPILER_TOKENOPERATOR_H

#include <memory>
#include <iostream>

#include "Token.h"
#include "../EnumRegistry.h"

namespace Operators {
    extern const std::shared_ptr<EnumRegistry::EnumRegistry> operatorRegistry;
    extern const EnumRegistry::RegistryItem equals;
    extern const int nOperators;
}

namespace Token {
    class TokenOperator : public Token::Token {
        public:
            const EnumRegistry::RegistryItem registryItem;
            TokenOperator(EnumRegistry::RegistryItem registryItemArg, int srcPosArg) : registryItem(registryItemArg), Token(srcPosArg) {}
            std::string getName() const {return "Operator Token(ID:"+std::to_string(registryItem.ID)
                                                +"; operand:\""+registryItem.str+"\")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenOperatorConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENOPERATOR_H