#ifndef LLVM_IR_COMPILER_SNESInstructionChangeProcessorStatus_H
#define LLVM_IR_COMPILER_SNESInstructionChangeProcessorStatus_H

#include "SNESInstruction.h"
#include "AddressingModes.h"

namespace {
    class SNESInstructionChangeProcessorStatus : public SNESAssembly::SNESInstruction {
        public:
            const AddressingModes::ImmediateForced8Bit operand;
            SNESInstructionChangeProcessorStatus(unsigned int operandArg) : operand(operandArg) {}
            virtual std::string getName() const override { return "SNES Instruction: ChangeProcessorStatus"; }
            virtual std::string getOpcode() const {
                throw std::runtime_error("SNESInstructionChangeProcessorStatus::getOpcode was not overriden in a subclass");
            }
            virtual std::string getStringRepresentation() const override { return getOpcode()+" "+operand.getStringRepresentation(); }
    };
}

namespace SNESAssembly {
    class SNESInstructionSetProcessorStatusBits : public SNESInstructionChangeProcessorStatus {
        public:
            SNESInstructionSetProcessorStatusBits(unsigned int operandArg) : SNESInstructionChangeProcessorStatus(operandArg) {}
            virtual std::string getName() const override { return "SNESInstruction: SetProcessorStatusBits"; }
            virtual std::string getOpcode() const override { return "SEP"; }
    };

    class SNESInstructionResetProcessorStatusBits : public SNESInstructionChangeProcessorStatus {
        public:
            SNESInstructionResetProcessorStatusBits(unsigned int operandArg) : SNESInstructionChangeProcessorStatus(operandArg) {}
            virtual std::string getName() const override { return "SNESInstruction: ResetProcessorStatusBits"; }
            virtual std::string getOpcode() const override { return "REP"; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionChangeProcessorStatus_H
