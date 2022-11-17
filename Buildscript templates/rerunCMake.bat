@echo off
goto Template
set ProjectFolder=REPLACE
set LLVMFolder=REPLACE

rmdir /s /q "%LLVMFolder%\llvm\lib\Target\SNES"
mkdir "%LLVMFolder%\llvm\lib\Target\SNES"
xcopy /v /q /s /y "%ProjectFolder%\SNES" "%LLVMFolder%\llvm\lib\Target\SNES"

set oldPath=%cd%
cd "%LLVMFolder%build"
cmake -DLLVM_ENABLE_PROJECTS="clang" -DCMAKE_BUILD_TYPE=Release -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=SNES -G  "Unix Makefiles" ../llvm
cd "%oldPath%"

rmdir /s /q "%LLVMFolder%\llvm\lib\Target\SNES"

:Template
echo "Folder names need to be filled in!"
pause