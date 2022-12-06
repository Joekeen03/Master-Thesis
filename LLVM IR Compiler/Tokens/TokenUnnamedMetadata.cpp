#include "TokenUnnamedMetadata.h"


namespace Token {
    TokenizeResult TokenUnnamedMetadataConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int currPos = startPos;
        int nextPosAfterToken = -1;
        bool success = false;
        int ID = -1;
        try {
            if ((*fileData)[currPos] == '!') {
                currPos++;
                if (std::isdigit((*fileData)[currPos])) {
                    TokenLib::numberParseResult result = TokenLib::extractNumber(fileData, currPos);
                    nextPosAfterToken = result.second;
                    ID = result.first;
                    success = true;
                    // FIXME is it safe to assume it's a valid token regardless of what character terminated the tokenization?
                    // I.e. would encountering an 'a' immediately after the identifier not invalidate the tokenization itself?
                    // result = TokenizeResult(TokenGlobalIdentifier(identifier), currPos);
                }
            }
        } catch (...) { // Out of bounds on fileData

        }
        return success ? TokenizeResult(std::make_shared<TokenUnnamedMetadata>(ID, startPos), nextPosAfterToken)
                        : TokenizeResult();
    }
}
