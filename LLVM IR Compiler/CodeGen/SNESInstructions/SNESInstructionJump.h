#ifndef LLVM_IR_COMPILER_SNESInstructionJump_H
#define LLVM_IR_COMPILER_SNESInstructionJump_H

#include <string>

#include "SNESInstruction.h"
#include "../NameMangling.h"

namespace SNESAssembly {
    // Jump to line; only accepts mangled code block labels, in absolute addressing mode.
    class SNESInstructionJumpAbsolute : public SNESInstruction {
        public:
            const CodeGen::MangledCodeBlockLabel jumpTarget;
            SNESInstructionJumpAbsolute(CodeGen::MangledCodeBlockLabel jumpTargetArg) : jumpTarget(jumpTargetArg) {}
            virtual std::string getName() const override { return "SNES Instruction: Jump (Absolute)"; }
            virtual std::string getStringRepresentation() const override { return "JMP "+jumpTarget.mangledLabel; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionJump_H
