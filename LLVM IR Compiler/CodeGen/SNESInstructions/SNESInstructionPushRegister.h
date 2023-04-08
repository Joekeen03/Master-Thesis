#ifndef LLVM_IR_COMPILER_SNESInstructionPushRegister_H
#define LLVM_IR_COMPILER_SNESInstructionPushRegister_H

#include "SNESInstruction.h"
#include "../../EnumRegistry.h"

namespace {
    class SNESInstructionPushRegister : public SNESAssembly::SNESInstruction {
        public:
            SNESInstructionPushRegister() {}
            virtual std::string getName() const { return "SNESInstruction: Push Register"; }
            virtual std::string getStringRepresentation() const override {
                throw std::runtime_error("SNESInstructionPushRegister::getStringRepresentation wasn't overridden by a subclass.");
            }
    };
}

namespace SNESAssembly {
    class SNESInstructionPushAccumulator : public SNESInstructionPushRegister {
        public:
            virtual std::string getName() const { return "SNESInstruction: Push Accumulator"; }
            virtual std::string getStringRepresentation() const override { return "PHA"; }
    };

    class SNESInstructionPushDirectPage : public SNESInstructionPushRegister {
        public:
            virtual std::string getName() const { return "SNESInstruction: Push Direct Page Register"; }
            virtual std::string getStringRepresentation() const override { return "PHD"; }
    };
    
    class SNESInstructionPushProgramBank : public SNESInstructionPushRegister {
        public:
            virtual std::string getName() const { return "SNESInstruction: Push Program Bank Register"; }
            virtual std::string getStringRepresentation() const override { return "PHK"; }
    };

    class SNESInstructionPushIndexRegisterX : public SNESInstructionPushRegister {
        public:
            virtual std::string getName() const { return "SNESInstruction: Push Index Register X"; }
            virtual std::string getStringRepresentation() const override { return "PHX"; }
    };

    class SNESInstructionPushIndexRegisterY : public SNESInstructionPushRegister {
        public:
            virtual std::string getName() const { return "SNESInstruction: Push Index Register Y"; }
            virtual std::string getStringRepresentation() const override { return "PHY"; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionPushRegister_H
