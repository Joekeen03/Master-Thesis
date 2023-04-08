#ifndef LLVM_IR_COMPILER_LocalsMap_H
#define LLVM_IR_COMPILER_LocalsMap_H

#include <utility>
#include <memory>
#include <map>
#include <string>

#include "../Types/Type.h"
#include "../lib/General.h"
#include "../Expressions/ExpressionLocalIdentifier.h"
#include "../Expressions/ExpressionLocalNamedIdentifier.h"
#include "../Expressions/ExpressionLocalUnnamedIdentifier.h"

namespace CodeGen { // FIXME use a more appropriate namespace?

    // variable's type, variable's position relative to stack pointer (for callee)
    using LocalsMapValue = std::pair<const std::shared_ptr<const Types::Type>, const int>;

    using LocalsTypeListItem = std::pair<const std::shared_ptr<const Expressions::ExpressionLocalIdentifier>, const std::shared_ptr<const Types::Type>>;
    using LocalsTypeList = std::vector<const LocalsTypeListItem>;
    using LocalsTypeListConstPtr = std::shared_ptr<const LocalsTypeList>;

    // Class that stores where each local variable is in the stack frame, relative to the hardware stack pointer.
    class LocalsMap {
        private:
            LocalsMap(std::string functionNameArg, std::shared_ptr<const std::map<const std::string, const LocalsMapValue>> namedLocalsArg,
                        std::shared_ptr<const std::map<const int, const LocalsMapValue>> unnamedLocalsArg, LocalsMapValue returnEntryArg)
                        : functionName(functionNameArg), namedLocals(namedLocalsArg),
                        unnamedLocals(unnamedLocalsArg), returnEntry(returnEntryArg) {
            }
        public:
            const std::string functionName;
            const LocalsMapValue returnEntry;
            // Main reason I have separate maps for named/unnamed locals is that:
            //  -Not sure if an integer surrounded by quotes (which usually define a named local) counts as an unnamed or named
            //      local, or if there are other ways to define named locals which are just numbers.
            //  -If this is possible, I'm not sure how to set up Compare for identifiers so I can use a generic
            //      ExpressionLocalIdentifier as the main key type, with named integer and unnamed local identifiers w/ the same
            //      integer value counting as distinct keys
            const std::shared_ptr<const std::map<const std::string, const LocalsMapValue>> namedLocals;
            const std::shared_ptr<const std::map<const int, const LocalsMapValue>> unnamedLocals;

            LocalsMapValue getLocalEntry(std::shared_ptr<const Expressions::ExpressionIdentifier> identifier) const;
            LocalsMapValue getLocalEntry(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> identifier) const;
            LocalsMapValue getLocalEntry(const Expressions::ExpressionLocalUnnamedIdentifier identifier) const;
            LocalsMapValue getLocalEntry(const Expressions::ExpressionLocalNamedIdentifier identifier) const;

            static std::shared_ptr<LocalsMap> createLocalsMap(std::string functionName, LocalsTypeListConstPtr paramLocals,
                                                                LocalsTypeListConstPtr instructionLocals,
                                                                std::shared_ptr<const Types::Type> returnType);

    };
}

#endif // LLVM_IR_COMPILER_LocalsMap_H
