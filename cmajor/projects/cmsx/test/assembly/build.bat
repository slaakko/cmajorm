@echo off
cmsxas -v echo.s 
cmsxlink -v -o=echo echo.o
cmsxas -v hello.s
cmsxlink -v -o=hello hello.o
