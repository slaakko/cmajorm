#	Makefile for Cmajor compiler & tools, system library, tool programs and examples.
#	Run with nmake from Visual Studio 2019 x64 Native Tools Command Prompt

all: compiler cppbert sys olddevenv
	inst.bat

compiler: buildcomp install

buildcomp:
	MSBuild -m -verbosity:minimal ..\cmajor.sln -p:Configuration="Debug"
	MSBuild -m -verbosity:minimal ..\cmajor.sln -p:Configuration="Release"
	
cppbert:
	cd ../rts/build/gcc && mingw32-make
	cd ../rts/build/gcc && mingw32-make install
	
sys:
	cmc -u -v ../system/platform/windows/System.cms
	cmc -u -v -bt=1 -c=release ../system/platform/windows/System.cms
	cppcmc -u -v -st ../system/platform/windows/System.cms
	cppcmc -u -v -st -c=release ../system/platform/windows/System.cms

cm:
	cmc -u -v ../projects/cm/Cm.cms
	cmc -u -v -bt=1 -c=release ../projects/cm/Cm.cms
	
cmlean:
	cmc -u -v ../projects/cm/Cm.Lean.cms
	cmc -u -v -bt=1 -c=release ../projects/cm/Cm.Lean.cms
	
tools:
	cmc -u -v ../projects/tools/soulcm/scmpg/scmpg.cmp
	cmc -u -v -c=release ../projects/tools/soulcm/scmpg/scmpg.cmp
	cmc -u -v ../projects/tools/soulcm/scmlg/scmlg.cmp
	cmc -u -v -c=release ../projects/tools/soulcm/scmlg/scmlg.cmp
	cmc -u -v ../projects/tools/soulcm/scm2html/scm2html.cmp
	cmc -u -v -c=release ../projects/tools/soulcm/scm2html/scm2html.cmp
	cmc -u -v ../projects/tools/spring/spring.cmp
	cmc -u -v -c=release ../projects/tools/spring/spring.cmp
	cmc -u -v ../projects/tools/supd/supd.cmp
	cmc -u -v -c=release ../projects/tools/supd/supd.cmp

examples:
	cmc -u -v ../projects/examples/examples.cms
	cmc -u -v -c=release ../projects/examples/examples.cms

cmsx:
	cmc -u -v -st -bt=1 ../projects/cmsx/cmsx.cms
	cmc -u -v -c=release -st -bt=1 ../projects/cmsx/cmsx.cms

preinstallation_check:
	MSBuild -m -verbosity:minimal ..\installation\checkcmajor\checkcmajor.vcxproj -p:Configuration="Debug"
	MSBuild -m -verbosity:minimal ..\installation\checkcmajor\checkcmajor.vcxproj -p:Configuration="Release"

plain_package:
	wingstall -v --create-package ..\..\setup\plain\cmajor.xml
	
plain_setup:	
	wingstall -v --make-setup ..\..\setup\plain\cmajor.bin
	MSBuild -m -verbosity:minimal ..\..\setup\plain\program\setup.vcxproj -p:Configuration="Debug"
	MSBuild -m -verbosity:minimal ..\..\setup\plain\program\setup.vcxproj -p:Configuration="Release"

deflate_package:
	wingstall -v --create-package ..\..\setup\deflate\cmajor.xml
	
deflate_setup:	
	wingstall -v --make-setup ..\..\setup\deflate\cmajor.bin
	MSBuild -m -verbosity:minimal ..\..\setup\deflate\program\setup.vcxproj -p:Configuration="Debug"
	MSBuild -m -verbosity:minimal ..\..\setup\deflate\program\setup.vcxproj -p:Configuration="Release"

bzip2_package:
	wingstall -v --create-package ..\..\setup\bzip2\cmajor.xml
	
bzip2_setup:	
	wingstall -v --make-setup ..\..\setup\bzip2\cmajor.bin
	MSBuild -m -verbosity:minimal ..\..\setup\bzip2\program\setup.vcxproj -p:Configuration="Debug"
	MSBuild -m -verbosity:minimal ..\..\setup\bzip2\program\setup.vcxproj -p:Configuration="Release"
	
packages: preinstallation_check plain_package deflate_package bzip2_package
	
setups: plain_setup deflate_setup bzip2_setup
	cminst -v ..\..\setup\plain\program\x64\Debug\setupd.exe ..\..\setup\plain\bin
	cminst -v ..\..\setup\plain\program\x64\Release\setup.exe ..\..\setup\plain\bin
	cminst -v ..\..\setup\deflate\program\x64\Debug\setupd.exe ..\..\setup\deflate\bin
	cminst -v ..\..\setup\deflate\program\x64\Release\setup.exe ..\..\setup\deflate\bin
	cminst -v ..\..\setup\bzip2\program\x64\Debug\setupd.exe ..\..\setup\bzip2\bin
	cminst -v ..\..\setup\bzip2\program\x64\Release\setup.exe ..\..\setup\bzip2\bin

packages_and_setups: packages setups

olddevenv:
	MSBuild -m -verbosity:minimal ..\..\devenv\devenv.sln -p:Configuration="Debug"
	MSBuild -m -verbosity:minimal ..\..\devenv\devenv.sln -p:Configuration="Release"
	
install:
	inst.bat
