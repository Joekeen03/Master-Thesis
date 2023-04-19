#ifndef LLVM_IR_COMPILER_SNESAssemblyComment_H
#define LLVM_IR_COMPILER_SNESAssemblyComment_H

#include <string>

namespace SNESAssembly {
    // Represents an 65c816 ASM comment.
    // Example: SNESAssemblyComment("Fruit") -> '; Fruit'
    class SNESAssemblyComment {
        public:
            const std::string comment;
            SNESAssemblyComment() {}
            SNESAssemblyComment(std::string commentArg) : comment(commentArg) {}
            inline std::string getStringRepresentation() const { return convertToCommentString(comment); }
            static inline std::string convertToCommentString(std::string rawComment) { return "; "+rawComment;}
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyComment_H
