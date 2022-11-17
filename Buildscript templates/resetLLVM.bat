@echo off
goto Template
set ProjectFolder=REPLACE
set LLVMFolder=REPLACE

echo Deleting LLVM folder
rmdir /s /q "%LLVMFolder%"
git clone -b main --single-branch https://github.com/llvm/llvm-project.git
mkdir "%LLVMFolder%build"

rerunCMake.bat
build.bat
pause

:Template
echo "Folder names need to be filled in!"
pause