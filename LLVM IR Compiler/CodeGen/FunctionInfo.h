#ifndef LLVM_IR_COMPILER_FunctionInfo_H
#define LLVM_IR_COMPILER_FunctionInfo_H

#include <string>
#include <memory>

#include "LocalsMap.h"

namespace CodeGen {
    class FunctionInfo {
        public:
            const std::string functionName;
            FunctionInfo(std::string functionNameArg) : functionName(functionNameArg) {}
    };
    
    class DefinedFunctionInfo : public FunctionInfo {
        public:
            const std::shared_ptr<const LocalsMap> localsMap;
            DefinedFunctionInfo(std::string functionName, std::shared_ptr<const LocalsMap> localsMapArg)
                                : FunctionInfo(functionName), localsMap(localsMapArg) {}
    };
}

#endif // LLVM_IR_COMPILER_FunctionInfo_H
