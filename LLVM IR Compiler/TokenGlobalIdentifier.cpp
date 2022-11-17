#include "TokenGlobalIdentifier.h"

namespace {
    inline bool isNamedIdentifierStartChar(char c) {
        return std::isalpha(c) || c == '-' || c == '$' || c == '.' || c == '_';
    }
}

namespace Token {
    TokenizeResult TokenGlobalIdentifierConstructor::tokenize(BasicArray::BasicCharArray* fileData, int currPos) {
        int newPos = currPos;
        TokenizeResult result;
        try {
            if ((*fileData)[newPos] == '@') {
                newPos++;
                if ((*fileData)[newPos] == '"') { // Double-quoted named identifier
                    newPos++;
                    char currChar;
                    std::string identifier;
                    bool tokenize = true, failed = false;
                    while (tokenize) {
                        currChar = (*fileData)[newPos++];
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
                        newPos = currPos;
                    } else {
                        result = TokenizeResult(TokenGlobalIdentifier(identifier), newPos);
                    }
                } else { // Unquoted identifier
                    std::string identifier;
                    char currChar = (*fileData)[newPos];
                    if (isNamedIdentifierStartChar(currChar)) { // Named identifier?
                        identifier += currChar;
                        newPos++;
                        bool tokenize = true;
                        while (tokenize) {
                            currChar = (*fileData)[newPos++];
                            if (isNamedIdentifierStartChar(currChar) || std::isdigit(currChar)) {
                                identifier += currChar;
                            } else {
                                tokenize = false;
                            }
                        }
                        result = TokenizeResult(TokenGlobalIdentifier(identifier), newPos);
                    } else if (std::isdigit(currChar)) {
                        identifier += currChar;
                        newPos++;
                        bool tokenize = true;
                        while (tokenize) {
                            currChar = (*fileData)[newPos++];
                            if (std::isdigit(currChar)) {
                                identifier += currChar;
                            } else {
                                tokenize = false;
                            }
                        }
                        // FIXME is it save to assume it's a valid token regardless of what character terminated the tokenization? I.e. would encountering
                        //  an 'a' immediately after the identifier not invalidate the tokenization itself?
                        result = TokenizeResult(TokenGlobalIdentifier(identifier), newPos);
                    }
                }
            }
        } catch (...) { // Went out of bounds on the array
            newPos = currPos;
        }
        return result;
    }
}