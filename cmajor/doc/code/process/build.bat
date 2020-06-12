@echo off
cmc -u -v testprog.cmp
xcopy /Y bin\debug\testprog.exe .
cmc -u -v ProcessTest.cmp
xcopy /Y bin\debug\ProcessTest.exe .
