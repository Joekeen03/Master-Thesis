#ifndef LLVM_IR_COMPILER_EXPRESSIONFunctionCodeBlock_H
#define LLVM_IR_COMPILER_EXPRESSIONFunctionCodeBlock_H

#include <string>

#include "Expression.h"
#include "Instructions/Instruction.h"

namespace Expressions {
    class ExpressionFunctionCodeBlock : public Expression {
        public:
            const std::string label;
            const std::shared_ptr<std::vector<std::shared_ptr<const Instructions::Instruction>>> instructions;
            const std::shared_ptr<Instructions::Instruction> terminator;
            ExpressionFunctionCodeBlock(std::string labelArg,
                                        std::shared_ptr<std::vector<std::shared_ptr<const Instructions::Instruction>>> instructionsArg,
                                        std::shared_ptr<Instructions::Instruction> terminatorArg)
                                            : label(labelArg), instructions(instructionsArg), terminator(terminatorArg) {}
            std::string getName() { return "ExpressionFunctionCodeBlock"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONFunctionCodeBlock_H
