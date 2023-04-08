#ifndef LLVM_IR_COMPILER_SNESAssemblySegmentInstructionChunk_H
#define LLVM_IR_COMPILER_SNESAssemblySegmentInstructionChunk_H

#include <optional>

#include "SNESAssemblySegment.h"
#include "SNESAssemblyLineComment.h"
#include "SNESAssemblyLineInstruction.h"

namespace SNESAssembly {
    // Represents a chunk of SNES-compatible 65c816 ASM code, which contains a series of instructions (with optional trailing
    //  comments), and a leading comment describing the instruction group.
    class SNESAssemblySegmentInstructionChunk : public SNESAssemblySegment {
        public:
            const std::optional<SNESAssemblyLineComment> leadingComment;
            const std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyLineInstruction>>> instructions;
            SNESAssemblySegmentInstructionChunk(std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyLineInstruction>>> instructionsArg)
                                                : instructions(instructionsArg) {}
            SNESAssemblySegmentInstructionChunk(SNESAssemblyLineComment leadingCommentArg, std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyLineInstruction>>> instructionsArg)
                                                : leadingComment(leadingCommentArg), instructions(instructionsArg) {}
            SNESAssemblySegmentInstructionChunk(std::string rawLeadingComment, std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyLineInstruction>>> instructionsArg)
                                                : leadingComment(rawLeadingComment), instructions(instructionsArg) {}
                                                
            virtual std::shared_ptr<const std::vector<const std::string>> getASMLines() const override;
    };

}

#endif // LLVM_IR_COMPILER_SNESAssemblySegmentInstructionChunk_H
