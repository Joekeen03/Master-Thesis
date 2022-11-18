#include "TokenParenthesis.h"

namespace Token {
    TokenizeResult TokenParenthesisConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int currPos = startPos;
        bool success = false;
        bool left = true;
        char currChar = (*fileData)[currPos++];
        switch (currChar) {
            case ')':
                left = false;
                success = true;
                break;
            case '(':
                success = true;
                break;
            default:
                break;
        }
        return success ? TokenizeResult(std::make_shared<TokenParenthesis>(left), currPos) : TokenizeResult();
    }
}