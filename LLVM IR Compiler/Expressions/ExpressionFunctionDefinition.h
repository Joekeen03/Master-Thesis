#ifndef LLVM_IR_COMPILER_EXPRESSIONFunctionDefinition_H
#define LLVM_IR_COMPILER_EXPRESSIONFunctionDefinition_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionFunctionDefinition : public Expression {
        public:
            ExpressionFunctionDefinition() {}
            std::string getName() { return "ExpressionFunctionDefinition"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONFunctionDefinition_H
