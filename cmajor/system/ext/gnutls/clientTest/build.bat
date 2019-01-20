@echo off
gcc -g "-I../include" -c clientTest.c -o clientTest.o
gcc -g "-L../lib" clientTest.o -lws2_32 -lgnutls.dll -o clientTest.exe
