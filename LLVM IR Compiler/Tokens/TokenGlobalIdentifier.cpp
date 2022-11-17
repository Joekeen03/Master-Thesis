#include "TokenGlobalIdentifier.h"

#include <iostream>

namespace {
    inline bool isNamedIdentifierStartChar(char c) {
        return std::isalpha(c) || c == '-' || c == '$' || c == '.' || c == '_';
    }
}

namespace Token {
    TokenizeResult TokenGlobalIdentifierConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        std::cout << "Attempting to create a global identifier token" << '\n';
        int currPos = startPos;
        bool succeeded = false;
        std::string identifier = "";
        /**
         * FIXME: What I'd like to do is have a "result" variable which is initialized to some "failed" value (e.g. TokenizeResult result;),
         *          and then if parsing succeeds, assign a "succcesful" value to it, like I would in Python or Java - except, I don't want to
         *          use pointers for this. Unfortunately, it seems that b/c TokenizeResult has const members (and an abstract data type for one
         *          of them), it doesn't work?
        */
        try {
            if ((*fileData)[currPos] == '@') {
                currPos++;
                if ((*fileData)[currPos] == '"') { // Double-quoted named identifier
                    currPos++;
                    char currChar;
                    bool tokenize = true, failed = false;
                    while (tokenize) {
                        currChar = (*fileData)[currPos++];
                        if (currChar == '"') {
                            tokenize = false;
                        } else if (currChar == '\n') {
                            tokenize = false;
                            failed = true;
                        } else {
                            identifier += currChar;
                        }
                    }
                    if (failed) {
                        currPos = startPos;
                    } else {
                        succeeded = true;
                    }
                } else { // Unquoted identifier
                    char currChar = (*fileData)[currPos];
                    if (isNamedIdentifierStartChar(currChar)) { // Named identifier?
                        identifier += currChar;
                        currPos++;
                        bool tokenize = true;
                        while (tokenize) {
                            currChar = (*fileData)[currPos++];
                            if (isNamedIdentifierStartChar(currChar) || std::isdigit(currChar)) {
                                identifier += currChar;
                            } else {
                                tokenize = false;
                            }
                        }
                        succeeded = true;
                    } else if (std::isdigit(currChar)) {
                        identifier += currChar;
                        currPos++;
                        bool tokenize = true;
                        while (tokenize) {
                            currChar = (*fileData)[currPos++];
                            if (std::isdigit(currChar)) {
                                identifier += currChar;
                            } else {
                                tokenize = false;
                            }
                        }
                        succeeded = true;
                        // FIXME is it safe to assume it's a valid token regardless of what character terminated the tokenization? I.e. would encountering
                        //  an 'a' immediately after the identifier not invalidate the tokenization itself?\
                        result = TokenizeResult(TokenGlobalIdentifier(identifier), currPos);
                    }
                }
            }
        } catch (...) { // Went out of bounds on the array
        
        }
        return succeeded ? TokenizeResult(TokenGlobalIdentifier(identifier), currPos) : TokenizeResult();
    }
}