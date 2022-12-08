#include "TokenTypeReservedWord.h"

namespace TypeReservedWords {
    std::shared_ptr<EnumRegistry::EnumRegistry> typeReservedWordRegistry = std::make_shared<EnumRegistry::EnumRegistry>("types");
    const EnumRegistry::RegistryItem typeVoid = typeReservedWordRegistry->registerItem("void");
    const EnumRegistry::RegistryItem typePtr = typeReservedWordRegistry->registerItem("ptr");
    extern const int nTypeReservedWords = typeReservedWordRegistry->finalize();
}

namespace Tokens {
    TokenizeResult TokenTypeReservedWordConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        bool success;
        int currPos = startPos;
        std::string word = "";
        try {
            bool buildString = true;
            do {
                char currChar = (*fileData)[currPos];
                if (std::isalpha(currChar) || std::isdigit(currChar) || currChar == '_') {
                    word += currChar;
                    currPos++;
                } else {
                    buildString = false;
                }
            } while (buildString);
        } catch (...) { // Out of bounds on fileData

        }
        int wordID = 0;
        Types::Type identifiedType;
        while (!success && wordID<TypeReservedWords::typeReservedWordRegistry->size()) {
            if (TypeReservedWords::typeReservedWordRegistry->getItem(wordID).str == word) {
                success = true;
            } else {
                wordID++;
            }
        }
        return success ? TokenizeResult(std::make_shared<TokenTypeReservedWord>(TypeReservedWords::typeReservedWordRegistry->getItem(wordID), startPos), currPos) : TokenizeResult();
    }
}
