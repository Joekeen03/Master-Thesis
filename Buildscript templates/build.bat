@echo off
goto Template
set ProjectFolder=REPLACE
set LLVMFolder=REPLACE

rmdir /s /q "%LLVMFolder%\llvm\lib\Target\SNES"
mkdir "%LLVMFolder%\llvm\lib\Target\SNES"
xcopy /v /q /s /y "%ProjectFolder%\SNES" "%LLVMFolder%\llvm\lib\Target\SNES"

set oldLoc=%cd%
cd /d "%LLVMFolder%\build"
make clang -j 04
cd /d "%oldLoc%"

rmdir /s /q "%LLVMFolder%\llvm\lib\Target\SNES"

:Template
echo "Folder names need to be filled in!"
pause