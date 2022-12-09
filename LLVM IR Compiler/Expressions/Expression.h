#ifndef LLVM_IR_COMPILER_EXPRESSION_H
#define LLVM_IR_COMPILER_EXPRESSION_H

#include <string>

namespace Expressions {
    template<typename T>
    using ptrVecPtrConst = std::shared_ptr<const std::vector<const std::shared_ptr<T>>>;
    
    class Expression {
        public:
            virtual std::string getName() const { return "Expression"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSION_H