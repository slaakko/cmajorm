@echo off
scmlg --system-x --verbose shlexer.lexer
sxcmc -u -v sh.cmp
sxcmc -u -v -c=release sh.cmp
