#include "TokenString.h"

namespace Token {
    TokenizeResult TokenStringConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int currPos = startPos;
        bool success = false;
        std::string str = "";
        try {
            if ((*fileData)[currPos++] == '"') { // Start of a string - I think all strings are double-quoted?
                bool buildString = true;
                do {
                    char currChar = (*fileData)[currPos++];
                    if (currChar == '\n') { // Strings can't span multiple lines
                        buildString = false;
                    } else if (currChar == '"') { // String end
                        buildString = false;
                        success = true;
                    } else {
                        str += currChar;
                    }
                } while (buildString);
            }
        } catch (...) { // Out of bounds on array

        }
        return success ? TokenizeResult(std::make_shared<TokenString>(str), currPos) : TokenizeResult();
    }
}