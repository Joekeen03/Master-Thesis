#ifndef LLVM_IR_COMPILER_TOKENKEYWORD_H
#define LLVM_IR_COMPILER_TOKENKEYWORD_H

#include <memory>
#include <iostream>
#include <cctype>

#include "Token.h"
#include "../EnumRegistry.h"

namespace ReservedWords {
    extern const std::shared_ptr<EnumRegistry::EnumRegistry> keywordRegistry;
    extern const EnumRegistry::RegistryItem source_filename;
    extern const EnumRegistry::RegistryItem target;
    extern const EnumRegistry::RegistryItem datalayout;
    extern const EnumRegistry::RegistryItem triple;
    extern const EnumRegistry::RegistryItem define;
    extern const EnumRegistry::RegistryItem dso_local;
    extern const EnumRegistry::RegistryItem noundef;
    extern const EnumRegistry::RegistryItem alloca;
    extern const EnumRegistry::RegistryItem align;
    extern const EnumRegistry::RegistryItem store;
    extern const EnumRegistry::RegistryItem ret;
    extern const EnumRegistry::RegistryItem attributes;
    extern const EnumRegistry::RegistryItem noinline;
    extern const EnumRegistry::RegistryItem mustprogress;
    extern const EnumRegistry::RegistryItem norecurse;
    extern const EnumRegistry::RegistryItem nounwind;
    extern const EnumRegistry::RegistryItem optnone;
    extern const EnumRegistry::RegistryItem uwtable;
    extern const int nKeywords;
}

namespace Tokens {
    class TokenKeyword : public Token {
        public:
            const EnumRegistry::RegistryItem registryItem;
            TokenKeyword(EnumRegistry::RegistryItem registryItemArg, int srcPosArg) : registryItem(registryItemArg), Token(srcPosArg) {}
            std::string getName() const {return "TokenKeyword(id:"+std::to_string(registryItem.ID)
                                                +"; keyword:"+registryItem.str+")"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenKeywordConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos);
    };
}

#endif // LLVM_IR_COMPILER_TOKENKEYWORD_H