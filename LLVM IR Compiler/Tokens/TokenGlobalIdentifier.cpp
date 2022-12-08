#include "TokenGlobalIdentifier.h"

#include <iostream>

#include "HelperFunctionsTokens.h"

namespace Tokens {
    TokenizeResult TokenGlobalIdentifierConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        return TokenLib::lexNamedIdentifier(fileData, startPos, '@', [](std::string str, int startPosLambda)
                                                        -> std::shared_ptr<Token> { return std::make_shared<TokenGlobalIdentifier>(str, startPosLambda);});
    }
}