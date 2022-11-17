@echo off
goto Template
set ProjectFolder=REPLACE
set LLVMFolder=REPLACE

"%LLVMFolder%\build\bin\clang" -v --target=snes "%ProjectFolder%\Test\SourcePrograms\mainOnly.c" -o "%ProjectFolder%\Test\Generated\mainOnly.asm"

:Template
echo "Folder names need to be filled in!"
pause