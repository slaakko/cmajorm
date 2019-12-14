@echo off
xcopy /Y /D ..\*.lexer ..\build
xcopy /Y /D ..\*.parser ..\build
xcopy /Y /D ..\*.spg ..\build
scmlg -v ../build/ParserFileLexer.lexer
scmlg -v ../build/ProjectFileLexer.lexer
scmpg -v ../build/Parsers.spg
