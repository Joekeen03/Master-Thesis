#include "TokenUnnamedCodeLabel.h"

#include "HelperFunctionsTokens.h"

namespace Tokens {
    TokenizeResult TokenUnnamedCodeLabelConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int currPos = startPos;
        auto idParseResult = TokenLib::extractUnsignedInteger(fileData, currPos);
        if (!idParseResult.success) {
            return TokenizeResult();
        }

        currPos = idParseResult.newPosition;
        if (!(fileData->positionInBounds(currPos) && (*fileData)[currPos] == ':')) {
            return TokenizeResult();
        }
        
        int nextPosAfterToken = currPos+1;
        return TokenizeResult(std::make_shared<TokenUnnamedCodeLabel>(startPos, idParseResult.result), nextPosAfterToken);
    }
}
