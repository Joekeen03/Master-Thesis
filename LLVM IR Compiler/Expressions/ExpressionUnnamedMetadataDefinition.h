#ifndef LLVM_IR_COMPILER_EXPRESSIONUnnamedMetadataDefinition_H
#define LLVM_IR_COMPILER_EXPRESSIONUnnamedMetadataDefinition_H

#include <string>

#include "Expression.h"
#include "ExpressionUnnamedMetadataIdentifier.h"
#include "ExpressionValue.h"

namespace Expressions {
    class ExpressionUnnamedMetadataDefinition : public Expression {
        public:
            const std::shared_ptr<const ExpressionUnnamedMetadataIdentifier> assignee;
            const bool distinct;
            const std::shared_ptr<const std::vector<const std::shared_ptr<const ExpressionValue>>> values;
            ExpressionUnnamedMetadataDefinition(std::shared_ptr<const ExpressionUnnamedMetadataIdentifier> assigneeArg, bool distinctArg,
                                                std::shared_ptr<const std::vector<const std::shared_ptr<const ExpressionValue>>> valuesArg)
                                                : assignee(assigneeArg), distinct(distinct), values(valuesArg) {}
            std::string getName() const { return "ExpressionUnnamedMetadataDefinition"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONUnnamedMetadataDefinition_H
