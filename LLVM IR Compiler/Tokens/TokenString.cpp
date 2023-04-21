#include "TokenString.h"

#include "HelperFunctionsTokens.h"

namespace Tokens {
    TokenizeResult TokenStringConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        TokenLib::stringParseResult stringResult = TokenLib::extractQuotedString(fileData, startPos);
        return stringResult.success ? TokenizeResult(std::make_shared<TokenString>(stringResult.result, startPos), stringResult.newPosition) : TokenizeResult();
    }
}