#include "TokenGlobalNamedIdentifier.h"

#include "HelperFunctionsTokens.h"

namespace Tokens {
    TokenizeResult TokenGlobalNamedIdentifierConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        return TokenLib::lexNamedIdentifier(fileData, startPos, '@', [](std::string str, int startPosLambda)
                                                        -> std::shared_ptr<Token> { return std::make_shared<TokenGlobalNamedIdentifier>(str, startPosLambda);});
    }
}
