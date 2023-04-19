#ifndef LLVM_IR_COMPILER_NameMangling_H
#define LLVM_IR_COMPILER_NameMangling_H

#include <string>
#include <memory>

#include "FunctionInfo.h"
#include "../Expressions/ExpressionFunctionCodeBlock.h"

namespace {
    const char* functionPrefix = "Function__";
    const char* codeBlockPrefix = "CodeBlock__";
}

namespace CodeGen {

    struct MangledCodeBlockLabel {
        const std::string mangledLabel;
    };

    struct MangledFunctionLabel {
        const std::string mangledLabel;
    };

    std::string inline mangleFunctionIdentifier(const std::shared_ptr<const FunctionInfo> functionInfo) {
        return "f_"+functionInfo->functionName;
    }

    std::string inline mangleCodeBlockIdentifier(const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock> codeBlock) {
        return "cb_"+codeBlock->label;
    }

    std::string inline mangleSpecialCodeBlockIdentifier(const std::string codeBlockIdentifier) {
        return "cbs_"+codeBlockIdentifier;
    }
    
    MangledFunctionLabel inline mangleFunctionLabel(const std::shared_ptr<const FunctionInfo> functionInfo) {
        return MangledFunctionLabel{functionPrefix+mangleFunctionIdentifier(functionInfo)};
    }
    
    MangledCodeBlockLabel inline mangleCodeBlockLabel(const std::shared_ptr<const FunctionInfo> functionInfo,
                                        const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock> codeBlock) {
        return MangledCodeBlockLabel{codeBlockPrefix+mangleFunctionIdentifier(functionInfo)+'_'
                                +mangleCodeBlockIdentifier(codeBlock)};
    }
    
    MangledCodeBlockLabel inline mangleSpecialCodeBlockLabel(const std::shared_ptr<const FunctionInfo> functionInfo,
                                                const std::string specialCodeBlockName) {
        return MangledCodeBlockLabel{codeBlockPrefix+mangleFunctionIdentifier(functionInfo)+'_'
                                +mangleSpecialCodeBlockIdentifier(specialCodeBlockName)};
    }

    MangledCodeBlockLabel inline mangleSetupCodeBlockLabel(const std::shared_ptr<const FunctionInfo> functionInfo) {
        return mangleSpecialCodeBlockLabel(functionInfo, "setup");
    }

    MangledCodeBlockLabel inline mangleCleanupCodeBlockLabel(const std::shared_ptr<const FunctionInfo> functionInfo) {
        return mangleSpecialCodeBlockLabel(functionInfo, "cleanup");
    }
}

#endif // LLVM_IR_COMPILER_NameMangling_H
