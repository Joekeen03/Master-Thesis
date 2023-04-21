#include "TokenTypeInteger.h"

#include "HelperFunctionsTokens.h"

namespace Tokens {
    TokenizeResult TokenTypeIntegerConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        auto prefixedNumberResult = TokenLib::extractPrefixedNumber(fileData, startPos, 'i');        
        return prefixedNumberResult.success ? TokenizeResult(std::make_shared<const TokenTypeInteger>(prefixedNumberResult.result, startPos), prefixedNumberResult.newPosition)
                                                        : TokenizeResult();
        throw new std::runtime_error("TokenTypeInteger not implemented.");
    }
}
