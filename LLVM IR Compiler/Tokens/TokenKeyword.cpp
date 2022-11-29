#include "TokenKeyword.h"

namespace ReservedWords {
    const std::shared_ptr<EnumRegistry::EnumRegistry> keywordRegistry = std::make_shared<EnumRegistry::EnumRegistry>("Keywords");
    const EnumRegistry::RegistryItem source_filename = keywordRegistry->registerItem("source_filename");
    const EnumRegistry::RegistryItem target = keywordRegistry->registerItem("target");
    const EnumRegistry::RegistryItem datalayout = keywordRegistry->registerItem("datalayout");
    const EnumRegistry::RegistryItem triple = keywordRegistry->registerItem("triple");
    const EnumRegistry::RegistryItem define = keywordRegistry->registerItem("define");
    const EnumRegistry::RegistryItem dso_local = keywordRegistry->registerItem("dso_local");
    const EnumRegistry::RegistryItem noundef = keywordRegistry->registerItem("noundef");
    const EnumRegistry::RegistryItem alloca = keywordRegistry->registerItem("alloca");
    const EnumRegistry::RegistryItem align = keywordRegistry->registerItem("align");
    const EnumRegistry::RegistryItem store = keywordRegistry->registerItem("store");
    const EnumRegistry::RegistryItem ptr = keywordRegistry->registerItem("ptr");
    const EnumRegistry::RegistryItem ret = keywordRegistry->registerItem("ret");
    const EnumRegistry::RegistryItem i32 = keywordRegistry->registerItem("i32");
    const EnumRegistry::RegistryItem attributes = keywordRegistry->registerItem("attributes");
    const EnumRegistry::RegistryItem noinline = keywordRegistry->registerItem("noinline");
    const EnumRegistry::RegistryItem mustprogress = keywordRegistry->registerItem("mustprogress");
    const EnumRegistry::RegistryItem norecurse = keywordRegistry->registerItem("norecurse");
    const EnumRegistry::RegistryItem nounwind = keywordRegistry->registerItem("nounwind");
    const EnumRegistry::RegistryItem optnone = keywordRegistry->registerItem("optnone");
    const EnumRegistry::RegistryItem uwtable = keywordRegistry->registerItem("uwtable");
    const int nKeywords = keywordRegistry->finalize();
}

namespace Token {
    TokenizeResult TokenKeywordConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int currPos = startPos;
        std::string word = "";
        bool success = false;
        try { // Extract a word
            bool buildString = true;
            do {
                char currChar = (*fileData)[currPos];
                if (std::isalpha(currChar) ||std::isdigit(currChar) || currChar == '_') {
                    word += currChar;
                    currPos++;
                } else {
                    buildString = false;
                }
            } while (buildString);
        } catch (...) { // Out of bounds on array

        }
        int wordID = 0;
        // std::cout << "Keyword string: '" << word << "'\n";
        while (!success && wordID<ReservedWords::keywordRegistry->size()) {
            if (ReservedWords::keywordRegistry->getItem(wordID).str == word) {
                success = true;
            } else {
                wordID++;
            }
        }
        
        return success ? TokenizeResult(std::make_shared<TokenKeyword>(ReservedWords::keywordRegistry->getItem(wordID), startPos), currPos) : TokenizeResult();
    }
}
