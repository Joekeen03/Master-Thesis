#include "TokenLibrary.h"

#include "TokenGlobalIdentifier.h"
#include "TokenComment.h"
#include "TokenKeyword.h"
#include "TokenOperator.h"
#include "TokenString.h"
#include "TokenParenthesis.h"
#include "TokenAttributeID.h"
#include "TokenCurlyBrace.h"

namespace TokenLibrary {
    std::unique_ptr<Token::TokenConstructor> tokenConstructors[] = {
        std::make_unique<Token::TokenGlobalIdentifierConstructor>(),
        std::make_unique<Token::TokenCommentConstructor>(),
        std::make_unique<Token::TokenKeywordConstructor>(),
        std::make_unique<Token::TokenOperatorConstructor>(),
        std::make_unique<Token::TokenStringConstructor>(),
        std::make_unique<Token::TokenParenthesisConstructor>(),
        std::make_unique<Token::TokenAttributeIDConstructor>(),
        std::make_unique<Token::TokenCurlyBraceConstructor>(),
    };
    const int nTokens = sizeof(tokenConstructors)/sizeof(tokenConstructors[0]);
}