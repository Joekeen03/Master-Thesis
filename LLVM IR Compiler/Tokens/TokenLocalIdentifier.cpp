#include "TokenLocalIdentifier.h"


namespace Token {
    TokenizeResult TokenLocalIdentifierConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        return tokenizeHelper(fileData, startPos, '%', [](std::string str)
                                                        -> std::shared_ptr<Token> { return std::make_shared<TokenLocalIdentifier>(str);});
    }
}
