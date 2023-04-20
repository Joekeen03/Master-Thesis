#ifndef LLVM_IR_COMPILER_INSTRUCTIONALLOCA_H
#define LLVM_IR_COMPILER_INSTRUCTIONALLOCA_H

#include <memory>

#include "InstructionNonTerminatorYieldsValue.h"
#include "../../Types/TypeSized.h"
#include "../../Types/TypePointer.h"

namespace Instructions {
    class InstructionAlloca : public InstructionNonTerminatorYieldsValue {
        public:
            const bool inalloca;
            const std::shared_ptr<const Types::TypeSized> allocationType;
            const int alignment;
            InstructionAlloca(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assigneeArg, bool inallocaArg,
                              std::shared_ptr<const Types::TypeSized> allocationTypeArg, int alignmentArg)
                                  : inalloca(inallocaArg), allocationType(allocationTypeArg), alignment(alignmentArg),
                                    InstructionNonTerminatorYieldsValue(assigneeArg) {}
            std::string getName() const { return "InstructionAlloca"; }
            virtual std::shared_ptr<const Types::Type> getYieldedType() const override {
                // TODO needs to be tweaked if I support the optional address space parameter.
                return std::make_shared<Types::TypePointer>(Types::LayoutAddressSpace::alloca);
            }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONALLOCA_H