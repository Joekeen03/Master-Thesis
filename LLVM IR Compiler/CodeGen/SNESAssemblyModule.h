#ifndef LLVM_IR_COMPILER_SNESAssemblyModule_H
#define LLVM_IR_COMPILER_SNESAssemblyModule_H

#include <memory>
#include <vector>
#include <optional>

#include "SNESAssemblyFunction.h"
#include "NameMangling.h"

namespace SNESAssembly {
    class SNESAssemblyModule {
        public:
            const std::optional<CodeGen::MangledFunctionLabel> mainFunction;
            const std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyFunction>>> functions;
            SNESAssemblyModule(std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyFunction>>> functionsArg,
                                CodeGen::MangledFunctionLabel mainFunctionArg) : functions(functionsArg), mainFunction(mainFunctionArg) {}
            SNESAssemblyModule(std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyFunction>>> functionsArg)
                                : functions(functionsArg) {}
            std::shared_ptr<const std::vector<const std::string>> getASMLines() const;
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyModule_H
