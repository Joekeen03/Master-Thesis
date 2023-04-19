#ifndef LLVM_IR_COMPILER_SNESAssemblyFunction_H
#define LLVM_IR_COMPILER_SNESAssemblyFunction_H

#include <memory>
#include <vector>

#include "SNESAssemblyCodeBlock.h"
#include "NameMangling.h"

namespace SNESAssembly {
    using codeBlockVectorPointer = std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyCodeBlock>>>;
    class SNESAssemblyFunction {
        public:
            const CodeGen::MangledFunctionLabel label;
            const codeBlockVectorPointer codeBlocks;
            SNESAssemblyFunction(CodeGen::MangledFunctionLabel labelArg, codeBlockVectorPointer codeBlocksArg): label(labelArg), codeBlocks(codeBlocksArg) {}
            std::shared_ptr<const std::vector<const std::string>> getASMLines() const;
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyFunction_H
