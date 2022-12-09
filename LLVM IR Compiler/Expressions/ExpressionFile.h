#ifndef LLVM_IR_COMPILER_EXPRESSIONFILE_H
#define LLVM_IR_COMPILER_EXPRESSIONFILE_H

#include <string>

#include "Expression.h"
#include "ExpressionSourceFile.h"
#include "ExpressionDataLayout.h"
#include "ExpressionTargetTriple.h"
#include "ExpressionFunctionDefinition.h"
#include "ExpressionUnnamedMetadataDefinition.h"
#include "ExpressionNamedMetadataDefinition.h"

namespace Expressions {
    class ExpressionFile : public Expression {
        public:
            const std::shared_ptr<const ExpressionSourceFile> sourceFile;
            const std::shared_ptr<const ExpressionDataLayout> dataLayout; // FIXME: Should it store the data layout components?
            const std::shared_ptr<const ExpressionTargetTriple> targetTriple;
            const ptrVecPtrConst<const ExpressionFunctionDefinition> functionDefinitions;
            const ptrVecPtrConst<const ExpressionUnnamedMetadataDefinition> unnamedMetadataDefinitions;
            const ptrVecPtrConst<const ExpressionNamedMetadataDefinition> namedMetadataDefinitions;
            ExpressionFile(std::shared_ptr<const ExpressionSourceFile> sourceFileArg,
                            std::shared_ptr<const ExpressionDataLayout> dataLayoutArg,
                            std::shared_ptr<const ExpressionTargetTriple> targetTripleArg,
                            ptrVecPtrConst<const ExpressionFunctionDefinition> functionDefinitionsArg,
                            ptrVecPtrConst<const ExpressionUnnamedMetadataDefinition> unnamedMetadataDefinitionsArg,
                            ptrVecPtrConst<const ExpressionNamedMetadataDefinition> namedMetadataDefinitionsArg)
                                    : sourceFile(sourceFileArg), dataLayout(dataLayoutArg), targetTriple(targetTripleArg),
                                        functionDefinitions(functionDefinitionsArg),
                                        unnamedMetadataDefinitions(unnamedMetadataDefinitionsArg),
                                        namedMetadataDefinitions(namedMetadataDefinitionsArg) {}
            std::string getName() const { return "ExpressionFile"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONFILE_H