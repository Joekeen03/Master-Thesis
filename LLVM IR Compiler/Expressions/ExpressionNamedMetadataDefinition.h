#ifndef LLVM_IR_COMPILER_EXPRESSIONNamedMetadataDefinition_H
#define LLVM_IR_COMPILER_EXPRESSIONNamedMetadataDefinition_H

#include <string>

#include "Expression.h"
#include "ExpressionNamedMetadataIdentifier.h"
#include "ExpressionUnnamedMetadataIdentifier.h"

namespace Expressions {
    class ExpressionNamedMetadataDefinition : public Expression {
        public:
            const std::shared_ptr<const ExpressionNamedMetadataIdentifier> assignee;
            const std::shared_ptr<const std::vector<const std::shared_ptr<const ExpressionUnnamedMetadataIdentifier>>> nodes;
            
            ExpressionNamedMetadataDefinition(std::shared_ptr<const ExpressionNamedMetadataIdentifier> assigneeArg,
                                                std::shared_ptr<const std::vector<const std::shared_ptr<const ExpressionUnnamedMetadataIdentifier>>> nodesArg)
                                                : assignee(assigneeArg), nodes(nodesArg) {}
            std::string getName() const { return "ExpressionNamedMetadataDefinition"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONNamedMetadataDefinition_H
