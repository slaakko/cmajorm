@echo off
set PDCURSER_SRCDIR=..
nmake -f ..\wincon\Makefile.vc WIDE=Y UTF8=Y
xcopy /Y pdcurses.lib ..\..\..\..\lib
