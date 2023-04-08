#ifndef LLVM_IR_COMPILER_SNESAssemblyModule_H
#define LLVM_IR_COMPILER_SNESAssemblyModule_H

#include <memory>
#include <vector>

#include "SNESAssemblyFunction.h"

namespace SNESAssembly {
    class SNESAssemblyModule {
        public:
            const bool isMainModule;
            const std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyFunction>>> functions;
            SNESAssemblyModule(std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyFunction>>> functionsArg,
                                bool isMainModuleArg) : functions(functionsArg), isMainModule(isMainModuleArg) {}
            SNESAssemblyModule(std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblyFunction>>> functionsArg)
                                : functions(functionsArg), isMainModule(false) {}
            std::shared_ptr<const std::vector<const std::string>> getASMLines() const;
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyModule_H
