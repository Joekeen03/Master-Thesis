#ifndef LLVM_IR_COMPILER_SNESAssemblySegment_H
#define LLVM_IR_COMPILER_SNESAssemblySegment_H

#include <string>
#include <vector>
#include <memory>

namespace SNESAssembly {
    class SNESAssemblySegment {
        public:
            virtual std::shared_ptr<const std::vector<const std::string>> getASMLines() const {
                throw std::runtime_error("SNESAssemblySegment::getASMLines was not implemented in a subclass.");
            };
            virtual int getBlockID() const { return -1; }
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblySegment_H
