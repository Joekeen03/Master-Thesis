#ifndef LLVM_IR_COMPILER_EXPRESSIONFunctionCodeBlock_H
#define LLVM_IR_COMPILER_EXPRESSIONFunctionCodeBlock_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionFunctionCodeBlock : public Expression {
        public:
            ExpressionFunctionCodeBlock() {}
            std::string getName() { return "ExpressionFunctionCodeBlock"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONFunctionCodeBlock_H
