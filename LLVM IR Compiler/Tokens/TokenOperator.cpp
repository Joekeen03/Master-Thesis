#include "TokenOperator.h"

namespace Operators {
    const std::shared_ptr<EnumRegistry::EnumRegistry> operatorRegistry = std::make_shared<EnumRegistry::EnumRegistry>("Operators");
    const EnumRegistry::RegistryItem equals = operatorRegistry->registerItem("=");
    const int nOperators = operatorRegistry->finalize();
}

namespace Token {
    TokenizeResult TokenOperatorConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int nextPosAfterChar = startPos;
        int operatorID = 0;
        bool success = false;
        while (!success && operatorID < Operators::nOperators) {
            EnumRegistry::RegistryItem op = Operators::operatorRegistry->getItem(operatorID);
            if (fileData->compareSubsectionToLiteral(startPos, op.str)) {
                success = true;
                nextPosAfterChar += op.str.size();
            } else {
                operatorID++;
            }
        }
        return success ? TokenizeResult(std::make_shared<TokenOperator>(Operators::operatorRegistry->getItem(operatorID), startPos), nextPosAfterChar) : TokenizeResult();
    }
}