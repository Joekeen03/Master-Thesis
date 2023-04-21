#include "TokenMetadataString.h"

#include "HelperFunctionsTokens.h"

namespace Tokens {
    TokenizeResult TokenMetadataStringConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        auto stringResult = (fileData->positionInBounds(startPos) && (*fileData)[startPos] == '!')
                                             ? TokenLib::extractQuotedString(fileData, startPos+1) : TokenLib::stringParseFAILED;
        return stringResult.success ? TokenizeResult(std::make_shared<TokenMetadataString>(stringResult.result, startPos), stringResult.newPosition)
                                   : TokenizeResult();
    }
}
