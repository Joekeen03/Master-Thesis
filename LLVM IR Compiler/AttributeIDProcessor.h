#ifndef LLVM_IR_COMPILER_ATTRIBUTEIDPROCESSOR_H
#define LLVM_IR_COMPILER_ATTRIBUTEIDPROCESSOR_H

#include <vector>
#include <string>

#include "Tokenizer.h"

namespace AttributeIDProcessor {
    class ProcessedTokens {
        public:
            const Tokenizer::tokensArrayPointer tokens;
            const std::shared_ptr<const std::vector<const Tokenizer::tokensArray>> attributeGroups;
            ProcessedTokens(Tokenizer::tokensArrayPointer tokensArg, std::shared_ptr<const std::vector<const Tokenizer::tokensArray>> attributeGroupsArg)
                            : tokens(tokensArg), attributeGroups(attributeGroupsArg) {}
    };
    /*A "pre-processor", which verifies that all attribute group definitions are valid (at module-level, proper syntax, not re-defined),
        and then substitutes all attribute IDs with the corresponding attribute group's tokens.
        Returns a ProcessedTokens object, containing an array of transformed tokens, an array indicating where each token
        came from (directly from source file, or from an attribute group), and an array of the attribute groups.
        I.e., if you have a source file like:
            noundef #0 noinline
            attributes #0 = { mustprogress norecurse }
        It turns into:
            tokens = {noundef, mustprogress, noinline, norecurse}
        Mapping data:
            mapping = {-1, 0, 0, -1}
            As the first & last tokens came from the source file (-1), and the other two came from attribute group #0.
        Attribute group:
            {{mustprogress, norecurse},}
    */
    class SubstitutedTokens {
        public:
            const Tokenizer::tokensArrayPointer tokens;
            const std::shared_ptr<std::vector<int>> mappings;
            const std::shared_ptr<const std::vector<const Tokenizer::tokensArray>> attributeGroups;
            SubstitutedTokens(Tokenizer::tokensArrayPointer tokensArg,
                              std::shared_ptr<std::vector<int>> mappingsArg,
                              std::shared_ptr<const std::vector<const Tokenizer::tokensArray>> attributeGroupsArg)
                              : tokens(tokensArg), mappings(mappingsArg), attributeGroups(attributeGroupsArg) {}
    };

    class AttributeIDProcessor {
        public:
            ProcessedTokens parseAttributeGroups(Tokenizer::tokensArrayPointer tokens);
            SubstitutedTokens substituteAttributeGroups(Tokenizer::tokensArrayPointer tokens);
    };

    class AttributeIDException : public std::runtime_error {
        public:
            AttributeIDException(std::string msg) : std::runtime_error(msg.c_str()) {}
    };
}

#endif // LLVM_IR_COMPILER_ATTRIBUTEIDPROCESSOR_H