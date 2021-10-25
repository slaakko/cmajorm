@echo off
cminst -v C:\llvm-11.0.0.src\build\Release\bin\llc.exe ..\bin
cminst -v C:\llvm-11.0.0.src\build\Release\bin\opt.exe ..\bin
cminst -v C:\llvm-11.0.0.src\build\Release\bin\llvm-dis.exe ..\bin
