#include "TokenOperator.h"

namespace Operators {
    const std::string operators[] = {
        "=",
    };
    const int nOperators = sizeof(operators)/sizeof(operators[0]);
}

namespace Token {
    TokenizeResult TokenOperatorConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int nextPosAfterChar = startPos;
        int operatorID = -1;
        bool success = false;
        while (!success && operatorID < Operators::nOperators) {
            if (fileData->compareSubsectionToLiteral(startPos, Operators::operators[operatorID])) {
                success = true;
                nextPosAfterChar += Operators::operators[operatorID].size();
            } else {
                operatorID++;
            }
        }
        return success ? TokenizeResult(std::make_shared<TokenOperator>(operatorID), nextPosAfterChar) : TokenizeResult();
    }
}