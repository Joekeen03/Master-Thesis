#include "TokenMetadataNodeStart.h"


namespace Token {
    TokenizeResult TokenMetadataNodeStartConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int currPos = startPos;
        bool success = false;
        try {
            success = ((*fileData)[currPos++] == '!' && (*fileData)[currPos++] == '{'); // Can they be whitespace separated?
        } catch (...) { // Out of bounds on fileData

        }
        return success ? TokenizeResult(std::make_shared<TokenMetadataNodeStart>(startPos), currPos) : TokenizeResult();
    }
}
