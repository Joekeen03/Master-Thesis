#ifndef LLVM_IR_COMPILER_EXPRESSIONMetadataIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONMetadataIdentifier_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionMetadataIdentifier : public Expression {
        public:
            ExpressionMetadataIdentifier() {}
            std::string getName() const { return "ExpressionMetadataIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONMetadataIdentifier_H
