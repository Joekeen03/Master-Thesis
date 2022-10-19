@echo off
set ProjectFolder=D:\JoesStuff\School Work\College\Graduate School\CSUN\Master Thesis\SNES Target\
set LLVMFolder=D:\JoesStuff\School Work\College\Graduate School\CSUN\Master Thesis\llvm-project\

"%LLVMFolder%\build\bin\clang" -v --target=snes "%ProjectFolder%\Test\SourcePrograms\mainOnly.c" -o "%ProjectFolder%\Test\Generated\mainOnly.asm"