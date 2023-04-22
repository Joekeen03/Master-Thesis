#ifndef LLVM_IR_COMPILER_SNESInstructionBranchAlways_H
#define LLVM_IR_COMPILER_SNESInstructionBranchAlways_H

#include "SNESInstruction.h"
#include "../NameMangling.h"

namespace SNESAssembly {    
    class SNESInstructionBranchAlways : public SNESInstruction {
        public:
            const CodeGen::MangledCodeBlockLabel target;
            SNESInstructionBranchAlways(CodeGen::MangledCodeBlockLabel targetArg) : target(targetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: SNESInstructionBranchAlways"; }
            virtual std::string getStringRepresentation() const override { return "BRA "+target.mangledLabel; }
    };
    

    class SNESInstructionBranchLongAlways : public SNESInstruction {
        public:
            const CodeGen::MangledCodeBlockLabel target;
            SNESInstructionBranchLongAlways(CodeGen::MangledCodeBlockLabel targetArg) : target(targetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: SNESInstructionBranchLongAlways"; }
            virtual std::string getStringRepresentation() const override { return "BRL "+target.mangledLabel; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionBranchAlways_H
