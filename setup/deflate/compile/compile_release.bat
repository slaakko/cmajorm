call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
msbuild -verbosity:minimal -t:Rebuild -p:Configuration="Release" "C:\work\cmajorm\setup\deflate\program\setup.vcxproj" > "C:\work\cmajorm\setup\deflate\compile\release_stdout.txt" 2> "C:\work\cmajorm\setup\deflate\compile\release_stderr.txt"
