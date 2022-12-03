#include "TokenBaseUnnamedIdentifier.h"

#include "HelperFunctionsTokens.h"

namespace Token {
    TokenizeResult TokenBaseUnnamedIdentifierConstructor::tokenizeHelper(BasicArray::BasicCharArray* fileData, int startPos, char identiferStartChar,
                                                        std::shared_ptr<Token> (*tokenCtor)(int ID, int srcPos)) {
        TokenLib::numberParseResult result = TokenLib::extractPrefixedNumber(fileData, startPos, identiferStartChar);
        return result != TokenLib::numberParseFAILED ? TokenizeResult(tokenCtor(result.first, startPos), result.second)
                                                        : TokenizeResult();
    }
}
