#ifndef LLVM_IR_COMPILER_SNESAssemblyFunction_H
#define LLVM_IR_COMPILER_SNESAssemblyFunction_H

#include <memory>
#include <vector>

#include "SNESAssemblyCodeBlock.h"

namespace SNESAssembly {
    using codeBlockVectorPointer = std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyCodeBlock>>>;
    class SNESAssemblyFunction {
        public:
            const std::string name;
            const codeBlockVectorPointer codeBlocks;
            SNESAssemblyFunction(std::string nameArg, codeBlockVectorPointer codeBlocksArg): name(nameArg), codeBlocks(codeBlocksArg) {}
            std::shared_ptr<const std::vector<const std::string>> getASMLines() const;
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyFunction_H
