#ifndef LLVM_IR_COMPILER_EXPRESSIONArgumentList_H
#define LLVM_IR_COMPILER_EXPRESSIONArgumentList_H

#include <string>

#include "Expression.h"

namespace Expression {
    class ExpressionArgumentList : public Expression {
        public:
            ExpressionArgumentList() {}
            std::string getName() { return "ExpressionArgumentList"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONArgumentList_H
