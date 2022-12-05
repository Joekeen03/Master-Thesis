#ifndef LLVM_IR_COMPILER_EXPRESSIONArgumentList_H
#define LLVM_IR_COMPILER_EXPRESSIONArgumentList_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionArgumentList : public Expression {
        public:
            ExpressionArgumentList() {}
            std::string getName() const { return "ExpressionArgumentList"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONArgumentList_H
