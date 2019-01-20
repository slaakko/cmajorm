@echo off
set PATH=C:\mingw-w64-8.1.0\mingw64\bin;%PATH%
gcc -c -O2 gmpintf.c -o gmpintf.o
gcc -shared gmpintf.o libgmp.a -o cmrt320gmp.dll
dlltool --verbose --output-def cmrt320gmp.def gmpintf.o
lib /MACHINE:X64 /DEF:cmrt320gmp.def /OUT:cmrt320gmp.lib
