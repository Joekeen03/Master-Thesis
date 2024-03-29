#include "TokenLocalUnnamedIdentifier.h"

#include "HelperFunctionsTokens.h"

namespace Tokens {
    TokenizeResult TokenLocalUnnamedIdentifierConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        return TokenLib::lexUnnamedIdentifier(fileData, startPos, '%', [](int ID, int startPosLambda)
                                                        -> std::shared_ptr<Token> { return std::make_shared<TokenLocalUnnamedIdentifier>(ID, startPosLambda);});
    }
}
