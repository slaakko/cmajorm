#	Makefile for Cmajor compiler & tools, system library, tool programs and examples.

all: compiler cppbert sys tools examples install

compiler: buildcomp install

buildcomp:
	MSBuild -m -verbosity:minimal ..\cmajor.sln -p:Configuration="Debug"
	MSBuild -m -verbosity:minimal ..\cmajor.sln -p:Configuration="Release"
	
cppbert:
	cd ../rts/build/gcc && mingw32-make
	cd ../rts/build/gcc && mingw32-make install
	
sys:
	cmc -u -v ../system/platform/windows/System.cms
	cmc -u -v -c=release ../system/platform/windows/System.cms
	cppcmc -u -v -st ../system/platform/windows/System.cms
	cppcmc -u -v -st -c=release ../system/platform/windows/System.cms
	
tools:
	cmc -u -v ../projects/cm/Cm.cms
	cmc -u -v -c=release ../projects/cm/Cm.cms
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

install:
	install.bat
