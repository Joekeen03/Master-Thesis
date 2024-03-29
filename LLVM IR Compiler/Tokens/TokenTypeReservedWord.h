#ifndef LLVM_IR_COMPILER_TOKENTypeReservedWord_H
#define LLVM_IR_COMPILER_TOKENTypeReservedWord_H

#include <memory>
#include <iostream>

#include "TokenType.h"

namespace TypeReservedWords {
    extern std::shared_ptr<EnumRegistry::EnumRegistry> typeReservedWordRegistry;
    extern const EnumRegistry::RegistryItem typeVoid;
    extern const EnumRegistry::RegistryItem typePtr;
    extern const EnumRegistry::RegistryItem typeLabel;
    extern const int nTypeReservedWords;
}

namespace Tokens {
    class TokenTypeReservedWord : public TokenType {
        public:
            const EnumRegistry::RegistryItem registryItem;
            explicit TokenTypeReservedWord(EnumRegistry::RegistryItem registryItemArg, int srcPosArg) : registryItem(registryItemArg), TokenType(srcPosArg) {}
            std::string getName() const {return "TokenTypeReservedWord(id:"+std::to_string(registryItem.ID)
                                                +"; keyword:"+registryItem.str+")"; ; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenTypeReservedWordConstructor : public TokenTypeConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENTypeReservedWord_H
