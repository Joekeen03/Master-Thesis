#include "TokenLocalIdentifier.h"


namespace Token {
    TokenizeResult TokenLocalIdentifierConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        // FIXME: Should the start position be baked into the lambda (uses startPos, doesn't take an int position as an argument)?
        // E.g. ...return std::make_shared<TokenLocalIdentifier>(str, startPos)...
        return tokenizeHelper(fileData, startPos, '%', [](std::string str, int startPosLambda)
                                                        -> std::shared_ptr<Token> { return std::make_shared<TokenLocalIdentifier>(str, startPosLambda);});
    }
}
