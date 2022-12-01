#include "TokenLocalUnnamedIdentifier.h"


namespace Token {
    TokenizeResult TokenLocalUnnamedIdentifierConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        return tokenizeHelper(fileData, startPos, '%', [](int ID, int startPosLambda)
                                                        -> std::shared_ptr<Token> { return std::make_shared<TokenLocalUnnamedIdentifier>(ID, startPosLambda);});
    }
}
