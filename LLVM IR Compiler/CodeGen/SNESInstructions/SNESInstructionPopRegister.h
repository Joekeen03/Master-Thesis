#ifndef LLVM_IR_COMPILER_SNESInstructionPopRegister_H
#define LLVM_IR_COMPILER_SNESInstructionPopRegister_H

#include "SNESInstruction.h"
#include "../../EnumRegistry.h"

namespace SNESAssembly {
    class SNESInstructionPopRegister : public SNESInstruction {
        public:
            SNESInstructionPopRegister() {}
            std::string getName() const override { return "SNES Instruction: PopRegister"; }
            std::string getStringRepresentation() const override {
                throw std::runtime_error("SNESInstructionPopRegister::getStringRepresentation wasn't overridden in a subclass.");
            }
    };
}

namespace SNESAssembly {
    class SNESInstructionPopAccumulator : public SNESInstructionPopRegister {
        public:
            virtual std::string getName() const { return "SNESInstruction: Pop Accumulator"; }
            virtual std::string getStringRepresentation() const override { return "PLA"; }
    };

    class SNESInstructionPopDirectPage : public SNESInstructionPopRegister {
        public:
            virtual std::string getName() const { return "SNESInstruction: Pop Direct Page Register"; }
            virtual std::string getStringRepresentation() const override { return "PLD"; }
    };
    
    class SNESInstructionPopProgramBank : public SNESInstructionPopRegister {
        public:
            virtual std::string getName() const { return "SNESInstruction: Pop Program Bank Register"; }
            virtual std::string getStringRepresentation() const override { return "PLK"; }
    };

    class SNESInstructionPopIndexRegisterX : public SNESInstructionPopRegister {
        public:
            virtual std::string getName() const { return "SNESInstruction: Pop Index Register X"; }
            virtual std::string getStringRepresentation() const override { return "PLX"; }
    };

    class SNESInstructionPopIndexRegisterY : public SNESInstructionPopRegister {
        public:
            virtual std::string getName() const { return "SNESInstruction: Pop Index Register Y"; }
            virtual std::string getStringRepresentation() const override { return "PLY"; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionPopRegister_H
