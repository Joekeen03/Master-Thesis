#include "TokenTypeInteger.h"

#include "HelperFunctionsTokens.h"

namespace Tokens {
    TokenizeResult TokenTypeIntegerConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        TokenLib::numberParseResult result = TokenLib::extractPrefixedNumber(fileData, startPos, 'i');        
        return result != TokenLib::numberParseFAILED ? TokenizeResult(std::make_shared<const TokenTypeInteger>(result.first, startPos), result.second)
                                                        : TokenizeResult();
        throw new std::runtime_error("TokenTypeInteger not implemented.");
    }
}
