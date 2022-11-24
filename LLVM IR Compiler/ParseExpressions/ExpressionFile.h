#ifndef LLVM_IR_COMPILER_EXPRESSIONFILE_H
#define LLVM_IR_COMPILER_EXPRESSIONFILE_H

#include <string>

#include "Expression.h"
#include "ExpressionSourceFile.h"
#include "ExpressionDataLayout.h"

namespace Expression {
    class ExpressionFile : public Expression {
        public:
            const std::shared_ptr<const ExpressionSourceFile> sourceFile; // FIXME: Should it store the data layout components?
            const std::shared_ptr<const ExpressionDataLayout> dataLayout; // FIXME: Should it store the data layout components?
            ExpressionFile(std::shared_ptr<const ExpressionSourceFile> sourceFileArg, std::shared_ptr<const ExpressionDataLayout> dataLayoutArg)
                                    : sourceFile(sourceFileArg), dataLayout(dataLayoutArg) {}
            std::string getName() { return "ExpressionFile"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONFILE_H