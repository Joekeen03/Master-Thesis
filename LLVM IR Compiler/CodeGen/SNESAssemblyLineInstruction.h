#ifndef LLVM_IR_COMPILER_SNESAssemblyLineInstruction_H
#define LLVM_IR_COMPILER_SNESAssemblyLineInstruction_H

#include <optional>
#include <memory>
#include <iostream>

#include "SNESAssemblyLine.h"
#include "SNESAssemblyComment.h"
#include "SNESInstructions/SNESInstruction.h"

namespace SNESAssembly {
    // Represents a single line of SNES-compatible 65c816 ASM code that has an instruction, and an optional trailing comment.
    // Examples:
    //      SNESAssemblyLineInstruction(instruction=push acc) -> 'PHA'
    //      SNESAssemblyLineInstruction(instruction=push acc, comment="Pineable") -> 'PHA ; Pineable'
    class SNESAssemblyLineInstruction : public SNESAssemblyLine {
        public:
            const std::shared_ptr<const SNESInstruction> instruction;
            // Would it make more sense to make this a pointer, which would be null if there's no comment, instead of using an
            //  "empty" comment?
            
            const std::optional<const SNESAssemblyComment> comment;
            SNESAssemblyLineInstruction(std::shared_ptr<const SNESInstruction> instructionArg) : instruction(instructionArg) {}
            SNESAssemblyLineInstruction(std::shared_ptr<const SNESInstruction> instructionArg, SNESAssemblyComment commentArg) : instruction(instructionArg), comment(commentArg) {}
            SNESAssemblyLineInstruction(std::shared_ptr<const SNESInstruction> instructionArg, std::string rawComment) : instruction(instructionArg), comment(SNESAssemblyComment(rawComment)) {}
            virtual std::string getStringRepresentation() const {
                auto line = instruction->getStringRepresentation();
                if (comment) {
                    line += " "+comment->getStringRepresentation();
                }
                return line;
            }
    };
}


#endif // LLVM_IR_COMPILER_SNESAssemblyLineInstruction_H
