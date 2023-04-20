#ifndef LLVM_IR_COMPILER_ProcessorFlags_H
#define LLVM_IR_COMPILER_ProcessorFlags_H

namespace CodeGen {
    // Flags for different processor status bits.
    constexpr extern unsigned int accumulatorModeFlag = 0x20;
    constexpr extern unsigned int indexRegisterModeFlag = 0x10;
}

#endif // LLVM_IR_COMPILER_ProcessorFlags_H
