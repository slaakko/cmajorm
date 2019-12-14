@echo off
xcopy /Y /D D:\work\soulng-project\soulng\util\*.cpp ..\soulng\util
xcopy /Y /D D:\work\soulng-project\soulng\util\*.hpp ..\soulng\util
xcopy /Y /D D:\work\soulng-project\soulng\util\*.vcxproj ..\soulng\util
xcopy /Y /D D:\work\soulng-project\soulng\lexer\*.cpp ..\soulng\lexer
xcopy /Y /D D:\work\soulng-project\soulng\lexer\*.hpp ..\soulng\lexer
xcopy /Y /D D:\work\soulng-project\soulng\lexer\*.vcxproj ..\soulng\lexer
xcopy /Y /D D:\work\soulng-project\soulng\lexer\Makefile ..\soulng\lexer
xcopy /Y /D D:\work\soulng-project\soulng\parser\*.cpp ..\soulng\parser
xcopy /Y /D D:\work\soulng-project\soulng\parser\*.hpp ..\soulng\parser
xcopy /Y /D D:\work\soulng-project\soulng\parser\*.vcxproj ..\soulng\parser
xcopy /Y /D D:\work\soulng-project\soulng\parser\Makefile ..\soulng\parser
xcopy /Y /D D:\work\soulng-project\sngxml\xml\*.cpp ..\sngxml\xml
xcopy /Y /D D:\work\soulng-project\sngxml\xml\*.hpp ..\sngxml\xml
xcopy /Y /D D:\work\soulng-project\sngxml\xml\*.vcxproj ..\sngxml\xml
xcopy /Y /D D:\work\soulng-project\sngxml\xml\*.parser ..\sngxml\xml
xcopy /Y /D D:\work\soulng-project\sngxml\xml\*.spg ..\sngxml\xml
xcopy /Y /D D:\work\soulng-project\sngxml\xml\Makefile ..\sngxml\xml
xcopy /Y /D D:\work\soulng-project\sngxml\dom\*.cpp ..\sngxml\dom
xcopy /Y /D D:\work\soulng-project\sngxml\dom\*.hpp ..\sngxml\dom
xcopy /Y /D D:\work\soulng-project\sngxml\dom\*.vcxproj ..\sngxml\dom
xcopy /Y /D D:\work\soulng-project\sngxml\dom\Makefile ..\sngxml\dom
xcopy /Y /D D:\work\soulng-project\sngxml\xpath\*.cpp ..\sngxml\xpath
xcopy /Y /D D:\work\soulng-project\sngxml\xpath\*.hpp ..\sngxml\xpath
xcopy /Y /D D:\work\soulng-project\sngxml\xpath\*.vcxproj ..\sngxml\xpath
xcopy /Y /D D:\work\soulng-project\sngxml\xpath\*.lexer ..\sngxml\xpath
xcopy /Y /D D:\work\soulng-project\sngxml\xpath\*.parser ..\sngxml\xpath
xcopy /Y /D D:\work\soulng-project\sngxml\xpath\*.spg ..\sngxml\xpath
xcopy /Y /D D:\work\soulng-project\sngxml\xpath\Makefile ..\sngxml\xpath
copy /Y ..\sngcm\ast\CompileUnit.cpp ..\sngcm\ast\CompileUnit.cpporg
copy /Y ..\sngcm\ast\CompileUnit.hpp ..\sngcm\ast\CompileUnit.hpporg
xcopy /Y /D D:\work\soulng-project\sngcm\ast\*.cpp ..\sngcm\ast
xcopy /Y /D D:\work\soulng-project\sngcm\ast\*.hpp ..\sngcm\ast
copy /Y ..\sngcm\ast\CompileUnit.cpporg ..\sngcm\ast\CompileUnit.cpp
copy /Y ..\sngcm\ast\CompileUnit.hpporg ..\sngcm\ast\CompileUnit.hpp
xcopy /Y /D D:\work\soulng-project\sngcm\ast\*.vcxproj ..\sngcm\ast
xcopy /Y /D D:\work\soulng-project\sngcm\ast\Makefile ..\sngcm\ast
xcopy /Y /D D:\work\soulng-project\sngcm\cmlexer\*.cpp ..\sngcm\cmlexer
xcopy /Y /D D:\work\soulng-project\sngcm\cmlexer\*.hpp ..\sngcm\cmlexer
xcopy /Y /D D:\work\soulng-project\sngcm\cmlexer\*.vcxproj ..\sngcm\cmlexer
xcopy /Y /D D:\work\soulng-project\sngcm\cmlexer\Makefile ..\sngcm\cmlexer
xcopy /Y /D D:\work\soulng-project\sngcm\cmlexer\*.lexer ..\sngcm\cmlexer
xcopy /Y /D D:\work\soulng-project\sngcm\cmparser\*.cpp ..\sngcm\cmparser
xcopy /Y /D D:\work\soulng-project\sngcm\cmparser\*.hpp ..\sngcm\cmparser
xcopy /Y /D D:\work\soulng-project\sngcm\cmparser\*.vcxproj ..\sngcm\cmparser
xcopy /Y /D D:\work\soulng-project\sngcm\cmparser\Makefile ..\sngcm\cmparser
xcopy /Y /D D:\work\soulng-project\sngcm\cmparser\*.parser ..\sngcm\cmparser
xcopy /Y /D D:\work\soulng-project\sngcm\cmparser\*.spg ..\sngcm\cmparser
