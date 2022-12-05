#ifndef LLVM_IR_COMPILER_EXPRESSIONFunctionDefinition_H
#define LLVM_IR_COMPILER_EXPRESSIONFunctionDefinition_H

#include <string>

#include "Expression.h"
#include "ExpressionFunctionHeaderPreName.h"
#include "ExpressionReturnType.h"
#include "ExpressionArgumentList.h"
#include "ExpressionFunctionHeaderPostName.h"
#include "ExpressionFunctionCodeBlock.h"

namespace Expressions {
    class ExpressionFunctionDefinition : public Expression {
        public:
            const std::shared_ptr<const ExpressionFunctionHeaderPreName> headerPreName;
            const std::shared_ptr<const ExpressionReturnType> returnType;
            const std::string name;
            const std::shared_ptr<const ExpressionArgumentList> argumentList;
            const std::shared_ptr<const ExpressionFunctionHeaderPostName> headerPostName;
            const std::shared_ptr<const std::vector<const std::shared_ptr<const ExpressionFunctionCodeBlock>>> codeBlocks;
            
            ExpressionFunctionDefinition(std::shared_ptr<const ExpressionFunctionHeaderPreName> headerPreNameArg,
                                            std::shared_ptr<const ExpressionReturnType> returnTypeArg, std::string nameArg,
                                            std::shared_ptr<const ExpressionArgumentList> argumentListArg,
                                            std::shared_ptr<const ExpressionFunctionHeaderPostName> headerPostNameArg,
                                            const std::shared_ptr<const std::vector<const std::shared_ptr<const ExpressionFunctionCodeBlock>>> codeBlocksArg)
                                            : headerPreName(headerPreNameArg), returnType(returnTypeArg), name(nameArg),
                                            argumentList(argumentListArg), headerPostName(headerPostNameArg), codeBlocks(codeBlocksArg) {}
            std::string getName() const { return "ExpressionFunctionDefinition"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONFunctionDefinition_H
