@echo off
set ProjectFolder=D:\JoesStuff\School Work\College\Graduate School\CSUN\Master Thesis\SNES Target\
set LLVMFolder=D:\JoesStuff\School Work\College\Graduate School\CSUN\Master Thesis\llvm-project\

rmdir /s /q "%LLVMFolder%\llvm\lib\Target\SNES"
mkdir "%LLVMFolder%\llvm\lib\Target\SNES"
xcopy /v /q /s /y "%ProjectFolder%\SNES" "%LLVMFolder%\llvm\lib\Target\SNES"

set oldLoc=%cd%
cd /d "%LLVMFolder%\build"
make clang
cd /d %oldLoc%