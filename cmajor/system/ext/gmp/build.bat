@echo off
set PATH=C:\mingw-w64-8.1.0\mingw64\bin;%PATH%
gcc -c -O2 gmpintf.c -o gmpintf.o
gcc -shared gmpintf.o libgmp.a -o cmrt340gmp.dll
dlltool --verbose --output-def cmrt340gmp.def gmpintf.o
lib /MACHINE:X64 /DEF:cmrt340gmp.def /OUT:cmrt340gmp.lib
