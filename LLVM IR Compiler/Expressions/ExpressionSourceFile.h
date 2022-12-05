#ifndef LLVM_IR_COMPILER_EXPRESSIONSOURCEFILE_H
#define LLVM_IR_COMPILER_EXPRESSIONSOURCEFILE_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionSourceFile : public Expression {
        public:
            const std::string fileName;
            ExpressionSourceFile(std::string fileNameArg) : fileName(fileNameArg) {}
            std::string getName() const { return "ExpressionSourceFile("+fileName+")"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONSOURCEFILE_H