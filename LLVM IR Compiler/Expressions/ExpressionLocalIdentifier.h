#ifndef LLVM_IR_COMPILER_EXPRESSIONLocalIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONLocalIdentifier_H

#include <string>

#include "ExpressionIdentifier.h"

namespace Expressions {
    class ExpressionLocalIdentifier : public ExpressionIdentifier {
        public:
            ExpressionLocalIdentifier() {}
            std::string getName() { return "ExpressionLocalIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONLocalIdentifier_H
