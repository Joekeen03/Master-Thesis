#ifndef LLVM_IR_COMPILER_AllInstructions_H
#define LLVM_IR_COMPILER_AllInstructions_H

#include <variant>

#include "InstructionStore.h"

#include "InstructionAlloca.h"
#include "InstructionLoad.h"
#include "InstructionAdd.h"
#include "InstructionTruncate.h"
#include "InstructionCallReturnValue.h"

#include "InstructionRetValue.h"
#include "InstructionBranchConditional.h"
#include "InstructionBranchUnconditional.h"

namespace Instructions {
    using InstructionVariant = std::variant<std::monostate,

                                            // Non-terminators that yield a value
                                            InstructionAlloca, InstructionLoad, InstructionAdd, InstructionTruncate,
                                            InstructionCallReturnValue,
                                            
                                            // Non-terminators that yield void
                                            InstructionStore,

                                            // Terminators
                                            InstructionRetValue, InstructionBranchConditional, InstructionBranchUnconditional>;

    using NonTerminators = std::variant<std::monostate,

                                        // Yields a value
                                        InstructionAlloca, InstructionLoad, InstructionAdd, InstructionTruncate,
                                        InstructionCallReturnValue,

                                        // Yields void
                                        InstructionStore>;

    using Terminators = std::variant<std::monostate,
                                     InstructionRetValue, InstructionBranchConditional, InstructionBranchUnconditional>;
}

#endif // LLVM_IR_COMPILER_AllInstructions_H
