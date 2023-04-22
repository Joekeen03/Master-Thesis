#ifndef LLVM_IR_COMPILER_INSTRUCTIONTERMINATORYIELDSVOIDTEMPLATE_H
#define LLVM_IR_COMPILER_INSTRUCTIONTERMINATORYIELDSVOIDTEMPLATE_H

#include <string>

#include "InstructionTerminatorYieldsVoid.h"

namespace Instructions {
    class InstructionTEMPLATE : public InstructionTerminatorYieldsVoid {
        public:
            InstructionTEMPLATE() {}
            std::string getName() const override { return "InstructionTEMPLATE"; }
            virtual std::string getStringRepresentation() const override {
                throw std::runtime_error("InstructionTEMPLATE::getStringRepresentation was not implemented.");
            }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONTERMINATORYIELDSVOIDTEMPLATE_H