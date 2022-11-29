#ifndef LLVM_IR_COMPILER_EXPRESSIONTEMPLATE_H
#define LLVM_IR_COMPILER_EXPRESSIONTEMPLATE_H

#include <string>

#include "Expression.h"

namespace Expression {
    class ExpressionTEMPLATE : public Expression {
        public:
            ExpressionTEMPLATE() {}
            std::string getName() { return "ExpressionTEMPLATE"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONTEMPLATE_H