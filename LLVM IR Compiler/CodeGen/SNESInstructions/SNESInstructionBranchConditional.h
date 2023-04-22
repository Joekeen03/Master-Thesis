#ifndef LLVM_IR_COMPILER_SNESInstructionBranchConditional_H
#define LLVM_IR_COMPILER_SNESInstructionBranchConditional_H

#include "SNESInstruction.h"
#include "../NameMangling.h"

namespace {
    class SNESInstructionBranch : public SNESAssembly::SNESInstruction {
        public:
            const CodeGen::MangledCodeBlockLabel target;
            SNESInstructionBranch(CodeGen::MangledCodeBlockLabel targetArg) : target(targetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: Branch"; }
            virtual std::string getStringRepresentation() const override { return getOpcode()+" "+target.mangledLabel; }
            virtual std::string getOpcode() const {
                throw std::runtime_error("SNESInstructionBranch::getOpcode wasn't overridden in a subclass.");
            }
    };
}

namespace SNESAssembly {
    class SNESInstructionBranchIfCarryClear : public SNESInstructionBranch {
        public:
            SNESInstructionBranchIfCarryClear(CodeGen::MangledCodeBlockLabel targetArg) : SNESInstructionBranch(targetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: SNESInstructionBranchIfCarryClear"; }
            virtual std::string getOpcode() const override { return "BCC"; }
    };
    using SNESInstructionBranchIfLessThan = SNESInstructionBranchIfCarryClear;
    
    class SNESInstructionBranchIfCarrySet : public SNESInstructionBranch {
        public:
            SNESInstructionBranchIfCarrySet(CodeGen::MangledCodeBlockLabel targetArg) : SNESInstructionBranch(targetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: SNESInstructionBranchIfCarrySet"; }
            virtual std::string getOpcode() const override { return "BCS"; }
    };
    using SNESInstructionBranchIfGreaterThan = SNESInstructionBranchIfCarrySet;
    
    class SNESInstructionBranchIfEqual : public SNESInstructionBranch {
        public:
            SNESInstructionBranchIfEqual(CodeGen::MangledCodeBlockLabel targetArg) : SNESInstructionBranch(targetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: SNESInstructionBranchIfEqual"; }
            virtual std::string getOpcode() const override { return "BEQ"; }
    };
    
    class SNESInstructionBranchIfMinus : public SNESInstructionBranch {
        public:
            SNESInstructionBranchIfMinus(CodeGen::MangledCodeBlockLabel targetArg) : SNESInstructionBranch(targetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: SNESInstructionBranchIfMinus"; }
            virtual std::string getOpcode() const override { return "BMI"; }
    };
    
    class SNESInstructionBranchIfNotEqual : public SNESInstructionBranch {
        public:
            SNESInstructionBranchIfNotEqual(CodeGen::MangledCodeBlockLabel targetArg) : SNESInstructionBranch(targetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: SNESInstructionBranchIfNotEqual"; }
            virtual std::string getOpcode() const override { return "BNE"; }
    };
    
    class SNESInstructionBranchIfPlus : public SNESInstructionBranch {
        public:
            SNESInstructionBranchIfPlus(CodeGen::MangledCodeBlockLabel targetArg) : SNESInstructionBranch(targetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: SNESInstructionBranchIfPlus"; }
            virtual std::string getOpcode() const override { return "BPL"; }
    };
    
    class SNESInstructionBranchIfOverflowClear : public SNESInstructionBranch {
        public:
            SNESInstructionBranchIfOverflowClear(CodeGen::MangledCodeBlockLabel targetArg) : SNESInstructionBranch(targetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: SNESInstructionBranchIfOverflowClear"; }
            virtual std::string getOpcode() const override { return "BVC"; }
    };

    class SNESInstructionBranchIfOverflowSet : public SNESInstructionBranch {
        public:
            SNESInstructionBranchIfOverflowSet(CodeGen::MangledCodeBlockLabel targetArg) : SNESInstructionBranch(targetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: SNESInstructionBranchIfOverflowSet"; }
            virtual std::string getOpcode() const override { return "BVS"; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionBranchConditional_H
