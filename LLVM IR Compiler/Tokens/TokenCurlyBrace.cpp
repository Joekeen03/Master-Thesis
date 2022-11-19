#include "TokenCurlyBrace.h"


namespace Token {
    TokenizeResult TokenCurlyBraceConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int currPos = startPos;
        bool success = false;
        bool left = true;
        char currChar = (*fileData)[currPos++];
        switch (currChar) {
            case '}':
                left = false;
                success = true;
                break;
            case '{':
                success = true;
                break;
            default:
                break;
        }
        return success ? TokenizeResult(std::make_shared<TokenCurlyBrace>(left), currPos) : TokenizeResult();
    }
}
