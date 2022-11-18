#include "TokenOperator.h"

namespace {
    const std::string operators[] = {
        "=",
    };
    const int nOperators = sizeof(operators)/sizeof(operators[0]);
}

namespace Token {
    TokenizeResult TokenOperatorConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int currPos = startPos;
        int operatorID = -1;
        bool success = false;
        for (int operatorID = 0; operatorID<nOperators && !success; operatorID++)
        {
            if (fileData->compareSubsectionToLiteral(startPos, operators[operatorID])) {
                success = true;
                currPos += operators[operatorID].size();
            }
        }
        return success ? TokenizeResult(std::make_shared<TokenOperator>(operatorID), currPos) : TokenizeResult();
    }
}