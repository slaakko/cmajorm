@echo off
cmsxas -v Main.s String.s System.s
cmsxar -v String.o System.o -o=System.a
