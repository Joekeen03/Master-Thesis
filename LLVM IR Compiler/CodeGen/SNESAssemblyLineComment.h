#ifndef LLVM_IR_COMPILER_SNESAssemblyLineComment_H
#define LLVM_IR_COMPILER_SNESAssemblyLineComment_H

#include "SNESAssemblyLine.h"
#include "SNESAssemblyComment.h"

namespace SNESAssembly {
    // Represents a line of SNES-compatible 65c816 ASM which holds a single comment.
    // Basically a SNESAssemblyLine wrapper for a comment.
    class SNESAssemblyLineComment : public SNESAssemblyLine {
        public:
            const SNESAssemblyComment comment;
            SNESAssemblyLineComment(SNESAssemblyComment commentArg) : comment(commentArg) {}
            SNESAssemblyLineComment(std::string rawComment) : comment(SNESAssemblyComment(rawComment)) {}
            virtual inline std::string getStringRepresentation() const { return comment.getStringRepresentation(); }
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyLineComment_H
