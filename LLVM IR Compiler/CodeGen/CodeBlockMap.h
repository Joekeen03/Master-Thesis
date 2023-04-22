#ifndef LLVM_IR_COMPILER_CodeBlockMap_H
#define LLVM_IR_COMPILER_CodeBlockMap_H

#include <string>
#include <map>
#include <memory>

#include "../Expressions/ExpressionLocalIdentifier.h"
#include "../Expressions/ExpressionLocalUnnamedIdentifier.h"
#include "../Expressions/ExpressionLocalNamedIdentifier.h"
#include "../lib/General.h"

namespace CodeGen {
    struct CodeBlockInfo {
        const std::string label;
        CodeBlockInfo(std::string labelArg) : label(labelArg) {}
    };

    class CodeBlockMap {
        private:
            const std::shared_ptr<const std::map<std::string, std::shared_ptr<const CodeBlockInfo>>> codeBlockInfoMap;
        public:
            CodeBlockMap(std::shared_ptr<const std::map<std::string, std::shared_ptr<const CodeBlockInfo>>> codeBlockInfoMapArg)
                            : codeBlockInfoMap(codeBlockInfoMapArg) {}

            std::shared_ptr<const CodeBlockInfo> getCodeblockInfo(std::string codeBlockLabel) const {
                return codeBlockInfoMap->at(codeBlockLabel);
            }

            std::shared_ptr<const CodeBlockInfo> getCodeblockInfo(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> codeBlockLabel) const {
                if (Lib::isType<Expressions::ExpressionLocalUnnamedIdentifier>(codeBlockLabel)) {
                    auto castIdentifier = std::dynamic_pointer_cast<const Expressions::ExpressionLocalUnnamedIdentifier>(codeBlockLabel);
                    return codeBlockInfoMap->at(std::to_string(castIdentifier->ID));
                } else if (Lib::isType<Expressions::ExpressionLocalNamedIdentifier>(codeBlockLabel)) {
                    auto castIdentifier = std::dynamic_pointer_cast<const Expressions::ExpressionLocalNamedIdentifier>(codeBlockLabel);
                    return codeBlockInfoMap->at(castIdentifier->name);
               }
            }
    };
}

#endif // LLVM_IR_COMPILER_CodeBlockMap_H
