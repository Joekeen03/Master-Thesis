#ifndef LLVM_IR_COMPILER_AllInstructions_H
#define LLVM_IR_COMPILER_AllInstructions_H

#include <variant>

#include "InstructionAlloca.h"
#include "InstructionRetValue.h"
#include "InstructionStore.h"

namespace Instructions {
    using InstructionVariant = std::variant<std::monostate, InstructionAlloca, InstructionRetValue, InstructionStore>;
    using NonTerminators = std::variant<std::monostate, InstructionAlloca, InstructionStore>;
    using Terminators = std::variant<std::monostate, InstructionRetValue>;
}

#endif // LLVM_IR_COMPILER_AllInstructions_H
