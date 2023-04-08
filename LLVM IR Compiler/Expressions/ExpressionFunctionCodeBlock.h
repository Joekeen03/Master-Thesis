#ifndef LLVM_IR_COMPILER_EXPRESSIONFunctionCodeBlock_H
#define LLVM_IR_COMPILER_EXPRESSIONFunctionCodeBlock_H

#include <string>

#include "Expression.h"
#include "Instructions/Instruction.h"
#include "Instructions/AllInstructions.h"

namespace Expressions {
    class ExpressionFunctionCodeBlock : public Expression {
        public:
            const std::string label;
            const std::shared_ptr<const std::vector<const std::shared_ptr<const Instructions::NonTerminators>>> instructions;
            const std::shared_ptr<const Instructions::Terminators> terminator;
            ExpressionFunctionCodeBlock(std::string labelArg,
                                        std::shared_ptr<const std::vector<const std::shared_ptr<const Instructions::NonTerminators>>> instructionsArg,
                                        std::shared_ptr<const Instructions::Terminators> terminatorArg)
                                            : label(labelArg), instructions(instructionsArg), terminator(terminatorArg) {}
            std::string getName() const { return "ExpressionFunctionCodeBlock"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONFunctionCodeBlock_H
