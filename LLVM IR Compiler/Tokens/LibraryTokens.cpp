#include "LibraryTokens.h"

#include "TokenGlobalNamedIdentifier.h"
#include "TokenLocalNamedIdentifier.h"
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
#include "TokenUnnamedMetadata.h"

namespace TokenLibrary {
    std::unique_ptr<Tokens::TokenConstructor> tokenConstructors[] = {
        std::make_unique<Tokens::TokenGlobalNamedIdentifierConstructor>(),
        std::make_unique<Tokens::TokenLocalNamedIdentifierConstructor>(),
        std::make_unique<Tokens::TokenLocalUnnamedIdentifierConstructor>(),
        std::make_unique<Tokens::TokenGlobalUnnamedIdentifierConstructor>(),
        std::make_unique<Tokens::TokenCommentConstructor>(),
        std::make_unique<Tokens::TokenKeywordConstructor>(),
        std::make_unique<Tokens::TokenOperatorConstructor>(),
        std::make_unique<Tokens::TokenStringConstructor>(),
        std::make_unique<Tokens::TokenParenthesisConstructor>(),
        std::make_unique<Tokens::TokenAttributeIDConstructor>(),
        std::make_unique<Tokens::TokenCurlyBraceConstructor>(),
        std::make_unique<Tokens::TokenNamedMetadataConstructor>(),
        std::make_unique<Tokens::TokenNumberLiteralConstructor>(),
        std::make_unique<Tokens::TokenMetadataNodeStartConstructor>(),
        std::make_unique<Tokens::TokenMetadataStringConstructor>(),
        std::make_unique<Tokens::TokenCommaConstructor>(),
        std::make_unique<Tokens::TokenTypeIntegerConstructor>(),
        std::make_unique<Tokens::TokenTypeReservedWordConstructor>(),
        std::make_unique<Tokens::TokenUnnamedMetadataConstructor>(),
    };
    const int nTokens = sizeof(tokenConstructors)/sizeof(tokenConstructors[0]);
    const std::unique_ptr<const Tokens::TokenEOFConstructor> EOFConstructor = std::make_unique<const Tokens::TokenEOFConstructor>();
}