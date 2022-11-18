#include "TokenKeyword.h"

namespace {
    const std::string keywords[] = {
        "source_filename",
        "target",
        "datalayout",
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
                char currChar = (*fileData)[currPos++];
                if (std::isalpha(currChar) || currChar == '_') {
                    word += currChar;
                } else {
                    buildString = false;
                }
            } while (buildString);
        } catch (...) { // Out of bounds on array

        }
        int wordID = -1;
        for (wordID = 0; wordID<nKeywords && !success; wordID++)
        {
            if (keywords[wordID] == word) {
                success = true;
            }
        }
        
        return success ? TokenizeResult(std::make_shared<TokenKeyword>(wordID), currPos) : TokenizeResult();
    }
}
