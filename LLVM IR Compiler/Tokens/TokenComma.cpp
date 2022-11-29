#include "TokenComma.h"


namespace Token {
    TokenizeResult TokenCommaConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        return (fileData->positionInBounds(startPos) && (*fileData)[startPos] == ',')
                ? TokenizeResult(std::make_shared<TokenComma>(startPos), startPos+1) : TokenizeResult();
    }
}
