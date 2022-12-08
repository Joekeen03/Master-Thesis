#include "TokenUnnamedMetadata.h"

#include "HelperFunctionsTokens.h"

namespace Token {
    TokenizeResult TokenUnnamedMetadataConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        return TokenLib::lexUnnamedIdentifier(fileData, startPos, '!', [](int ID, int startPosLambda)
                                                        -> std::shared_ptr<Token> { return std::make_shared<TokenUnnamedMetadata>(ID, startPosLambda);});
    }
}
