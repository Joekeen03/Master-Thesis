#include "TokenLibrary.h"

#include "TokenGlobalIdentifier.h"
#include "TokenComment.h"
#include "TokenSourceFile.h"

namespace TokenLibrary {
    std::unique_ptr<Token::TokenConstructor> tokenConstructors[] = {
        std::make_unique<Token::TokenGlobalIdentifierConstructor>(),
        std::make_unique<Token::TokenCommentConstructor>(),
        std::make_unique<Token::TokenSourceFileConstructor>(),
    };
    const int nTokens = sizeof(tokenConstructors)/sizeof(tokenConstructors[0]);
}