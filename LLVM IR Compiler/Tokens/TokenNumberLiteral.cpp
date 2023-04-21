#include "TokenNumberLiteral.h"

#include "HelperFunctionsTokens.h"

namespace Tokens {
    TokenizeResult TokenNumberLiteralConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        // Do number literals need to be whitespace separated from other tokens (e.g. global identifiers or operators)?
        // What if we encounter a letter right after the number - does that invalidate the number literal?
        // FIXME Should this tokenize signed numbers, or should it tokenize only unsigned numbers, with a separate token
        //  for the minus sign, which the parser then determining if the minus sign indicates a negative number, or
        //  something else.
        TokenLib::signedIntegerParseResult integerResult = TokenLib::extractSignedInteger(fileData, startPos);
        return integerResult.success ? TokenizeResult(std::make_shared<TokenNumberLiteral>(integerResult.result, startPos), integerResult.newPosition) : TokenizeResult();
    }
}
