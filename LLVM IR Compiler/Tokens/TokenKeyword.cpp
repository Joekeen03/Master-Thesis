#include "TokenKeyword.h"

namespace ReservedWords {
    const std::string keywords[] = {
        "source_filename",
        "target",
        "datalayout",
        "triple",
        "define",
        "dso_local",
        "noundef",
        "alloca",
        "align",
        "store",
        "ptr",
        "ret",
        "i32",
        "attributes",
        "noinline",
        "mustprogress",
        "norecurse",
        "nounwind",
        "optnone",
        "uwtable",
    };
    const int nKeywords = sizeof(keywords)/sizeof(keywords[0]);
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
        while (!success && wordID<ReservedWords::nKeywords) {
            if (ReservedWords::keywords[wordID] == word) {
                success = true;
            } else {
                wordID++;
            }
        }
        
        return success ? TokenizeResult(std::make_shared<TokenKeyword>(wordID), currPos) : TokenizeResult();
    }
}
