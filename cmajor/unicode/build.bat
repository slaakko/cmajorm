@echo off
IF NOT EXIST ucd.all.flat.xml powershell -command "Expand-Archive -Force '%~dp0ucd.all.flat.zip' '%~dp0'"
IF NOT EXIST cmajor_ucd.bin makecmajorucd
