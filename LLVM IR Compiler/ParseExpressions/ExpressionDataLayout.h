#ifndef LLVM_IR_COMPILER_EXPRESSIONDATALAYOUT_H
#define LLVM_IR_COMPILER_EXPRESSIONDATALAYOUT_H

#include <string>

#include "Expression.h"

namespace Expression {
    class ExpressionDataLayout : public Expression {
        public:
            const std::string dataLayout; // FIXME: Should it store the data layout components?
            ExpressionDataLayout(std::string dataLayoutArg) : dataLayout(dataLayoutArg) {}
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONDATALAYOUT_H