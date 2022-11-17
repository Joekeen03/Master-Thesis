#include "Token.h"
#include "TokenGlobalIdentifier.h"

namespace TokenLibrary {
    Token::TokenConstructor tokenConstructors[] = {
        Token::TokenGlobalIdentifierConstructor()
    };
    const int nTokens = sizeof(tokenConstructors)/sizeof(tokenConstructors[0]);
}