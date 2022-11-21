#include "TokenNumberLiteral.h"


namespace Token {
    TokenizeResult TokenNumberLiteralConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        bool success = false;
        int currPos = startPos;
        std::string numberRaw = "";
        try {
            bool buildNumber = true;
            do {
                char currChar = (*fileData)[currPos++];
                // Do number literals need to be whitespace separated from other tokens (e.g. global identifiers or operators)?
                // What if we encounter a letter right after the number - does that invalidate the number literal?
                if (std::isdigit(currChar)) {
                    numberRaw += currChar;
                } else {
                    buildNumber = false;
                }
            } while (buildNumber);
            success = numberRaw.size() > 0; // FIXME better way to handle this?
        } catch (...) { // Out of bounds on fileData

        }
        return success ? TokenizeResult(std::make_shared<TokenNumberLiteral>(std::stoi(numberRaw)), currPos) : TokenizeResult();
    }
}
