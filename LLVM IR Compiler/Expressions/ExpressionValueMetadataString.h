#ifndef LLVM_IR_COMPILER_EXPRESSIONValueMetadataString_H
#define LLVM_IR_COMPILER_EXPRESSIONValueMetadataString_H

#include <string>

#include "ExpressionValue.h"

namespace Expressions {
    class ExpressionValueMetadataString : public ExpressionValue {
        public:
            // Should this instead store an 'ExpressionMetadataString'? Not sure if this (a metadata node's 'value') is the
            // only place metadata strings can appear.
            const std::string str;
            ExpressionValueMetadataString(std::string strArg) : str(strArg) {}
            std::string getName() const { return "ExpressionValueMetadataString"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONValueMetadataString_H
