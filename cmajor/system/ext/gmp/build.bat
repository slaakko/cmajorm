@echo off
set PATH=C:\mingw-w64-8.1.0\mingw64\bin;%PATH%
gcc -c -O2 gmpintf.c -o gmpintf.o
gcc -shared gmpintf.o libgmp.a -o cmrt330gmp.dll
dlltool --verbose --output-def cmrt330gmp.def gmpintf.o
lib /MACHINE:X64 /DEF:cmrt330gmp.def /OUT:cmrt330gmp.lib
