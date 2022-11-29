#include "TokenNumberLiteral.h"

#include "HelperFunctionsTokens.h"

namespace Token {
    TokenizeResult TokenNumberLiteralConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        // Do number literals need to be whitespace separated from other tokens (e.g. global identifiers or operators)?
        // What if we encounter a letter right after the number - does that invalidate the number literal?
        TokenLib::numberParseResult result = TokenLib::extractNumber(fileData, startPos);
        return result.second != -1 ? TokenizeResult(std::make_shared<TokenNumberLiteral>(result.first, startPos), result.second) : TokenizeResult();
    }
}
