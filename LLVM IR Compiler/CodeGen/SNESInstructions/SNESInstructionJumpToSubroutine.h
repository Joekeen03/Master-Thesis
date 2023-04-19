#ifndef LLVM_IR_COMPILER_SNESInstructionJumpToSubroutine_H
#define LLVM_IR_COMPILER_SNESInstructionJumpToSubroutine_H

#include "SNESInstruction.h"
#include "../NameMangling.h"

namespace SNESAssembly {
    

    // Jump to subroutine; only accepts mangled function labels, in absolute addressing mode.
    class SNESInstructionJumpSubroutineAbsolute : public SNESInstruction {
        public:
            const CodeGen::MangledFunctionLabel jumpTarget;
            SNESInstructionJumpSubroutineAbsolute(CodeGen::MangledFunctionLabel jumpTargetArg) : jumpTarget(jumpTargetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: Jump Subroutine(Absolute)"; }
            virtual std::string getStringRepresentation() const override { return "JSR "+jumpTarget.mangledLabel; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionJumpToSubroutine_H
