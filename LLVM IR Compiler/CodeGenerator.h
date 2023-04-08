#ifndef LLVM_IR_COMPILER_CodeGenerator_H
#define LLVM_IR_COMPILER_CodeGenerator_H

#include <memory>
#include <string>
#include <vector>

#include "Expressions/ExpressionFile.h"
#include "Expressions/Instructions/Instruction.h"
#include "CodeGen/SNESInstructions/SNESInstruction.h"
#include "CodeGen/LocalsMap.h"
#include "CodeGen/SNESAssemblySegmentInstructionChunk.h"
#include "Expressions/Instructions/AllInstructions.h"

namespace CodeGen {
    class CodeGenerator {
        private:
            const std::shared_ptr<const Expressions::ExpressionFile> moduleFile;

            std::shared_ptr<LocalsMap> computeVariableLocations(std::shared_ptr<const Expressions::ExpressionFunctionDefinition> function);
            
            // Generates the SNES 65c816 instructions corresponding to the non-terminator LLVM IR instruction.
            // Uses a very simple memory model for the SNES 65c816 instructions, with the return value, parameters, and every
            //  LLVM IR local on the stack.
            template <typename T>
            std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> convertNonTerminatorInstruction(T instruction, std::shared_ptr<LocalsMap> localsMap);

            // Generates the SNES 65c816 instructions corresponding to the terminator LLVM IR instruction.
            // Uses a very simple memory model for the SNES 65c816 instructions, with the return value, parameters, and every
            //  LLVM IR local on the stack.
            template <typename T>
            std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> convertTerminatorInstruction(T instruction, std::shared_ptr<LocalsMap> localsMap);
        public:
            CodeGenerator(std::shared_ptr<const Expressions::ExpressionFile> moduleFileArg) : moduleFile(moduleFileArg) {}
            std::shared_ptr<const std::vector<const std::string>> generateCode();
            
            
            // Couldn't get Intellisense to show the constructors for T as the parameter options, like it does for std::make_shared<>
            // template<typename T, typename ...ArgTypes>
            // static inline std::shared_ptr<const SNESAssembly::SNESInstruction> createAndUpcast(ArgTypes&& ...args) { return std::shared_ptr<const T>(new T(std::forward<ArgTypes>(args)...)); }
    };
}

#endif // LLVM_IR_COMPILER_CodeGenerator_H
