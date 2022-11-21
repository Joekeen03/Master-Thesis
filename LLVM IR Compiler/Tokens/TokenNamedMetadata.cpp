#include "TokenNamedMetadata.h"

#include <cctype>

#include "HelperFunctionsTokens.h"

namespace Token {
    TokenizeResult TokenNamedMetadataConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int currPos = startPos;
        int nextPosAfterToken = currPos;
        std::string name = "";
        bool success = false;
        try {
            if ((*fileData)[currPos++] == '!') {
                char currChar = (*fileData)[currPos];
                if (TokenLib::isNamedIdentifierStartChar(currChar) || currChar == '\'') { // Named identifier?
                    // Can't just add the first character to the name, as it might be the start of an escape sequence.
                    bool buildName = true;
                    bool failed = false;
                    do {
                        currChar = (*fileData)[currPos];
                        if (currChar == '\'') { // Escape sequence
                            name += currChar;
                            currPos++;
                            // Escape sequences are of form \XY, where X & Y are hex digits
                            if (std::isxdigit((*fileData)[currPos]) && std::isxdigit((*fileData)[currPos+1])) {
                                name += (*fileData)[currPos++];
                                name += (*fileData)[currPos++];
                            } else { // Invalid escape sequence.
                                buildName = false;
                                failed = true;
                            }
                        } else if (TokenLib::isNamedIdentifierStartChar(currChar) || std::isdigit(currChar)) {
                            name += currChar;
                            currPos++;
                        } else {
                            nextPosAfterToken = currPos;
                            buildName = false;
                        }
                    } while (buildName);
                    success = !failed;

                } else if (std::isdigit(currChar)) {
                    TokenLib::numberParseResult result = TokenLib::extractNumber(fileData, currPos);
                    nextPosAfterToken = result.second;
                    name = std::to_string(result.first);
                    success = true;
                    // FIXME is it safe to assume it's a valid token regardless of what character terminated the tokenization?
                    // I.e. would encountering an 'a' immediately after the identifier not invalidate the tokenization itself?
                    // result = TokenizeResult(TokenGlobalIdentifier(identifier), currPos);
                }
            }
        } catch (...) { // Out of bounds on fileData

        }
        return success ? TokenizeResult(std::make_shared<TokenNamedMetadata>(name), currPos) : TokenizeResult();
    }
}
