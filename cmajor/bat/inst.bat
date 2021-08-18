@echo off
IF NOT EXIST cminst.exe xcopy /Y /D ..\x64\Release\cminst.exe .
IF NOT EXIST cmsnglexer.dll xcopy /Y /D ..\x64\Release\cmsnglexer.dll .
IF NOT EXIST cmsngrex.dll xcopy /Y /D ..\x64\Release\cmsngrex.dll .
IF NOT EXIST cmsngutil.dll xcopy /Y /D ..\x64\Release\cmsngutil.dll .
cminst -v ..\x64\Debug\*.exe ..\bin
cminst -v ..\x64\Release\*.exe ..\bin
cminst -v ..\x64\Debug\*.dll ..\bin
cminst -v ..\x64\Release\*.dll ..\bin
cminst -v ..\x64\Debug\*.lib ..\lib
cminst -v ..\x64\Release\*.lib ..\lib
cminst -v ..\cmcode\res\cmcodesmall.ico ..\bin
cminst -v C:\llvm-11.0.0.src\build\Release\bin\llc.exe ..\bin
cminst -v C:\llvm-11.0.0.src\build\Release\bin\opt.exe ..\bin
cminst -v C:\llvm-11.0.0.src\build\Release\bin\llvm-dis.exe ..\bin
cminst -v ..\task\bin\Release\CmajorTasks.dll ..\bin
cminst -v ..\projects\examples\HexDump\bin\release\hexdump.exe ..\bin
cminst -v ..\projects\examples\sted\bin\release\sted.exe ..\bin
cminst -v ..\projects\examples\ms\bin\release\ms.exe ..\bin
cminst -v ..\projects\examples\sgrep\bin\release\sgrep.exe ..\bin
cminst -v ..\projects\cmsx\cmsx\bin\release\cmsx.exe ..\bin
cminst -v ..\projects\cmsx\cmsxic\bin\release\cmsxic.exe ..\bin
cminst -v ..\projects\cmsx\cmsxas\bin\release\cmsxas.exe ..\bin
cminst -v ..\projects\cmsx\cmsxdump\bin\release\cmsxdump.exe ..\bin
cminst -v ..\projects\cmsx\cmsxlink\bin\release\cmsxlink.exe ..\bin
cminst -v ..\projects\cmsx\cmsxar\bin\release\cmsxar.exe ..\bin
cminst -v ..\projects\tools\supd\bin\release\supd.exe ..\bin
cminst -v ..\projects\tools\spring\bin\release\spring.exe ..\bin
cminst -v ..\projects\tools\soulcm\scm2html\bin\release\scm2html.exe ..\bin
cminst -v ..\projects\tools\soulcm\scmlg\bin\release\scmlg.exe ..\bin
cminst -v ..\projects\tools\soulcm\scmpg\bin\release\scmpg.exe ..\bin
cminst -v ..\..\devenv\cmdevenv\bin\x64\Release\*.exe ..\bin
cminst -v ..\..\devenv\cmdevenv\bin\x64\Release\*.exe.config ..\bin
cminst -v ..\..\devenv\cmdevenv\bin\x64\Release\*.dll ..\bin
