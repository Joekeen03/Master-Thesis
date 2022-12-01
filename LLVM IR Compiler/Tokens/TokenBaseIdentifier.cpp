#include "TokenBaseIdentifier.h"

#include <utility>

#include "HelperFunctionsTokens.h"

namespace Token {
    TokenizeResult TokenBaseIdentifierConstructor::tokenizeHelper(BasicArray::BasicCharArray* fileData, int startPos, char identiferStartChar,
                                                        std::shared_ptr<Token> (*tokenCtor)(std::string, int)) {
        int currPos = startPos;
        int firstCharAfterToken = currPos;
        bool succeeded = false;
        std::string identifier = "";
        /**
         * FIXME: What I'd like to do is have a "result" variable which is initialized to some "failed" value (e.g. TokenizeResult result;),
         *          and then if parsing succeeds, assign a "succcesful" value to it, like I would in Python or Java - except, I don't want to
         *          use pointers for this. Unfortunately, it seems that b/c TokenizeResult has const members (and an abstract data type for one
         *          of them), it doesn't work?
        */
        try {
            if ((*fileData)[currPos] == identiferStartChar) {
                currPos++;
                if ((*fileData)[currPos] == '"') { // Double-quoted named identifier
                    TokenLib::stringParseResult result = TokenLib::extractQuotedString(fileData, currPos);
                    if (result.second != -1) {
                        identifier = result.first;
                        firstCharAfterToken = result.second;
                        succeeded = true;
                    }
                } else { // Unquoted identifier
                    char currChar = (*fileData)[currPos];
                    if (TokenLib::isNamedIdentifierStartChar(currChar)) { // Named identifier?
                        identifier += currChar;
                        currPos++;
                        firstCharAfterToken = currPos;
                        bool tokenize = true;
                        while (tokenize) {
                            currChar = (*fileData)[currPos];
                            if (TokenLib::isNamedIdentifierStartChar(currChar) || std::isdigit(currChar)) {
                                identifier += currChar;
                                currPos++;
                                /** FIXME: Better way to handle this than selectively incrementing currPos, depending on
                                 *          whether or not the most recent character was part of the string? Just, the main reason
                                 *          I'm doing this is so, when I return a successful token, currPos is the location of the
                                 *          first character after the keyword string. But, this feels a bit unclear.
                                */
                            } else {
                                tokenize = false;
                                firstCharAfterToken = currPos;
                            }
                        }
                        succeeded = true;
                    }
                }
            }
        } catch (...) { // Went out of bounds on the array
        
        }
        return succeeded ? TokenizeResult(tokenCtor(identifier, startPos), firstCharAfterToken) : TokenizeResult();
    }
}
