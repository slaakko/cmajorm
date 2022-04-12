@echo off
scmlg --system-x --verbose container_file_lexer.lexer
sxcmc -u -v sx.cmp
sxcmc -u -v -c=release sx.cmp
