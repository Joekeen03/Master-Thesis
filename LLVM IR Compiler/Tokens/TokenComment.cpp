#include "TokenComment.h"

#include <iostream>

namespace Token {
    TokenizeResult TokenCommentConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int currPos = startPos;
        bool succeeded = false;
        std::string comment = "";
        try {
            if ((*fileData)[currPos] == ';') {
                try {
                    currPos++;
                    bool tokenize = true;
                    while (tokenize) {
                        char currChar = (*fileData)[currPos++];
                        if (currChar == '\n') {
                            tokenize = false;
                        } else {
                            comment += currChar;
                        }
                    }
                } catch (...) { // Encountered the end of the file

                }
                // Either encountered a newline, or the end of the file.
                succeeded = true;
            }
        } catch (...) { // Out-of-bounds on array before reading a ';'
            std::cout << "[WARN] TokenComment exceeded fileData array bounds on first character." << '\n';
        }
        return succeeded ? TokenizeResult(std::make_shared<TokenComment>(comment), currPos) : TokenizeResult();
    }
}