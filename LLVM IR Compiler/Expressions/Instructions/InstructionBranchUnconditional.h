#ifndef LLVM_IR_COMPILER_INSTRUCTIONTERMINATORYIELDSVOIDBranchUnconditional_H
#define LLVM_IR_COMPILER_INSTRUCTIONTERMINATORYIELDSVOIDBranchUnconditional_H

#include <string>

#include "InstructionTerminatorYieldsVoid.h"
#include "../ExpressionLocalIdentifier.h"

namespace Instructions {
    class InstructionBranchUnconditional : public InstructionTerminatorYieldsVoid {
        public:
            const std::shared_ptr<const Expressions::ExpressionLocalIdentifier> destination;
            InstructionBranchUnconditional(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> destinationArg)
                                            : destination(destinationArg) {}
            std::string getName() const override { return "InstructionBranchUnconditional"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONTERMINATORYIELDSVOIDBranchUnconditional_H
