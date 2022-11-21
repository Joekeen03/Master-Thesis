#include "TokenAttributeID.h"

#include "HelperFunctionsTokens.h"

namespace Token {
    TokenizeResult TokenAttributeIDConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        TokenLib::numberParseResult result = (fileData->positionInBounds(startPos) && (*fileData)[startPos] == '#')
                                             ? TokenLib::extractNumber(fileData, startPos+1) : TokenLib::numberParseFAILED;
        return result.second != -1 ? TokenizeResult(std::make_shared<TokenAttributeID>(result.first), result.second) : TokenizeResult();
    }
}
