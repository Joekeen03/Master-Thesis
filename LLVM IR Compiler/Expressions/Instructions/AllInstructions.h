#ifndef LLVM_IR_COMPILER_AllInstructions_H
#define LLVM_IR_COMPILER_AllInstructions_H

#include <variant>

#include "InstructionAlloca.h"
#include "InstructionRetValue.h"
#include "InstructionStore.h"
#include "InstructionLoad.h"
#include "InstructionAdd.h"

namespace Instructions {
    using InstructionVariant = std::variant<std::monostate,
                                            // Terminators
                                            InstructionRetValue,
                                            // Non-terminators that yield a value
                                            InstructionAlloca, InstructionLoad, InstructionAdd,
                                            // Non-terminators that yield void
                                            InstructionStore>;

    using NonTerminators = std::variant<std::monostate,
                                        // Yields a value
                                        InstructionAlloca, InstructionLoad, InstructionAdd,
                                        // Yields void
                                        InstructionStore>;

    using Terminators = std::variant<std::monostate, InstructionRetValue>;
}

#endif // LLVM_IR_COMPILER_AllInstructions_H
