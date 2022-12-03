#include "LibraryTokens.h"

#include "TokenGlobalIdentifier.h"
#include "TokenLocalIdentifier.h"
#include "TokenLocalUnnamedIdentifier.h"
#include "TokenGlobalUnnamedIdentifier.h"
#include "TokenComment.h"
#include "TokenKeyword.h"
#include "TokenOperator.h"
#include "TokenString.h"
#include "TokenParenthesis.h"
#include "TokenAttributeID.h"
#include "TokenCurlyBrace.h"
#include "TokenNamedMetadata.h"
#include "TokenNumberLiteral.h"
#include "TokenMetadataNodeStart.h"
#include "TokenMetadataString.h"
#include "TokenComma.h"
#include "TokenTypeInteger.h"
#include "TokenTypeReservedWord.h"

namespace TokenLibrary {
    std::unique_ptr<Token::TokenConstructor> tokenConstructors[] = {
        std::make_unique<Token::TokenGlobalIdentifierConstructor>(),
        std::make_unique<Token::TokenLocalIdentifierConstructor>(),
        std::make_unique<Token::TokenLocalUnnamedIdentifierConstructor>(),
        std::make_unique<Token::TokenGlobalUnnamedIdentifierConstructor>(),
        std::make_unique<Token::TokenCommentConstructor>(),
        std::make_unique<Token::TokenKeywordConstructor>(),
        std::make_unique<Token::TokenOperatorConstructor>(),
        std::make_unique<Token::TokenStringConstructor>(),
        std::make_unique<Token::TokenParenthesisConstructor>(),
        std::make_unique<Token::TokenAttributeIDConstructor>(),
        std::make_unique<Token::TokenCurlyBraceConstructor>(),
        std::make_unique<Token::TokenNamedMetadataConstructor>(),
        std::make_unique<Token::TokenNumberLiteralConstructor>(),
        std::make_unique<Token::TokenMetadataNodeStartConstructor>(),
        std::make_unique<Token::TokenMetadataStringConstructor>(),
        std::make_unique<Token::TokenCommaConstructor>(),
        std::make_unique<Token::TokenTypeIntegerConstructor>(),
        std::make_unique<Token::TokenTypeReservedWordConstructor>(),
    };
    const int nTokens = sizeof(tokenConstructors)/sizeof(tokenConstructors[0]);
}