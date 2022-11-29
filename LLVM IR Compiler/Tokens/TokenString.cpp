#include "TokenString.h"

#include "HelperFunctionsTokens.h"

namespace Token {
    TokenizeResult TokenStringConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        TokenLib::stringParseResult result = TokenLib::extractQuotedString(fileData, startPos);
        return result.second != -1 ? TokenizeResult(std::make_shared<TokenString>(result.first, startPos), result.second) : TokenizeResult();
    }
}