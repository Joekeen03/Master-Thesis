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

    std::string inline mangleRawFunctionIdentifier(const std::string functionIdentifier) {
        return "rf_"+functionIdentifier;
    }

    std::string inline mangleRawCodeBlockIdentifier(const std::string codeBlockIdentifier) {
        return "rcb_"+codeBlockIdentifier;
    }
    
    MangledFunctionLabel inline mangleFunctionLabel(const std::shared_ptr<const FunctionInfo> functionInfo) {
        return MangledFunctionLabel{functionPrefix+mangleFunctionIdentifier(functionInfo)};
    }
    
    MangledCodeBlockLabel inline mangleCodeBlockLabel(const std::shared_ptr<const FunctionInfo> functionInfo,
                                        const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock> codeBlock) {
        return MangledCodeBlockLabel{codeBlockPrefix+mangleFunctionIdentifier(functionInfo)+'_'
                                +mangleCodeBlockIdentifier(codeBlock)};
    }
    
    MangledCodeBlockLabel inline mangleRawCodeBlockLabel(const std::shared_ptr<const FunctionInfo> functionInfo,
                                                const std::string rawCodeBlockName) {
        return MangledCodeBlockLabel{codeBlockPrefix+mangleFunctionIdentifier(functionInfo)+'_'
                                +mangleRawCodeBlockIdentifier(rawCodeBlockName)};
    }

    MangledCodeBlockLabel inline mangleSetupCodeBlockLabel(const std::shared_ptr<const FunctionInfo> functionInfo) {
        return mangleRawCodeBlockLabel(functionInfo, "setup");
    }

    MangledCodeBlockLabel inline mangleCleanupCodeBlockLabel(const std::shared_ptr<const FunctionInfo> functionInfo) {
        return mangleRawCodeBlockLabel(functionInfo, "cleanup");
    }
}

#endif // LLVM_IR_COMPILER_NameMangling_H
