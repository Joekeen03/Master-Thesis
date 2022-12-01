#include "TokenBaseUnnamedIdentifier.h"

#include "HelperFunctionsTokens.h"

namespace Token {
    TokenizeResult TokenBaseUnnamedIdentifierConstructor::tokenizeHelper(BasicArray::BasicCharArray* fileData, int startPos, char identiferStartChar,
                                                        std::shared_ptr<Token> (*tokenCtor)(int ID, int srcPos)) {
        int currPos = startPos;
        int firstCharAfterToken = currPos;
        bool succeeded = false;
        int identifier = 0;
        /**
         * FIXME: What I'd like to do is have a "result" variable which is initialized to some "failed" value (e.g. TokenizeResult result;),
         *          and then if parsing succeeds, assign a "succcesful" value to it, like I would in Python or Java - except, I don't want to
         *          use pointers for this. Unfortunately, it seems that b/c TokenizeResult has const members (and an abstract data type for one
         *          of them), it doesn't work?
        */
        try {
            if ((*fileData)[currPos] == identiferStartChar) {
                currPos++;
                char currChar = (*fileData)[currPos];
                if (std::isdigit(currChar)) {
                    TokenLib::numberParseResult result = TokenLib::extractNumber(fileData, currPos);
                    firstCharAfterToken = result.second;
                    succeeded = true;
                }
            }
        } catch (...) { // Went out of bounds on the array
        
        }
        return succeeded ? TokenizeResult(tokenCtor(identifier, startPos), firstCharAfterToken) : TokenizeResult();
    }
}
