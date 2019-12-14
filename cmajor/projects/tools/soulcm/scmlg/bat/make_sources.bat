@echo off
xcopy /Y /D ..\*.lexer ..\build
xcopy /Y /D ..\*.parser ..\build
xcopy /Y /D ..\*.spg ..\build
scmlg -v ../build/LexerFileLexer.lexer
scmlg -v ../build/RegExLexer.lexer
scmpg -v ../build/Parsers.spg
