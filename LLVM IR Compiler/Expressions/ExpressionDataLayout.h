#ifndef LLVM_IR_COMPILER_EXPRESSIONDATALAYOUT_H
#define LLVM_IR_COMPILER_EXPRESSIONDATALAYOUT_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionDataLayout : public Expression {
        public:
            // FIXME: Should it store the data layout components?
            const std::string dataLayout;
            ExpressionDataLayout(std::string dataLayoutArg) : dataLayout(dataLayoutArg) {}
            std::string getName() { return "ExpressionDataLayout("+dataLayout+")"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONDATALAYOUT_H