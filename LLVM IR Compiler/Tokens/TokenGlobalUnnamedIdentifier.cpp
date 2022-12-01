#include "TokenGlobalUnnamedIdentifier.h"


namespace Token {
    TokenizeResult TokenGlobalUnnamedIdentifierConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        return tokenizeHelper(fileData, startPos, '@', [](int ID, int startPosLambda)
                                                        -> std::shared_ptr<Token> { return std::make_shared<TokenGlobalUnnamedIdentifier>(ID, startPosLambda);});
    }
}
