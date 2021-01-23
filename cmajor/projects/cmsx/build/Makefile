all: mnm sys cu prc sh

alld: mnm sysd cud prcd shd

sh:
	sxcmc --outdir=shell --verbose --config=release ../src/shell/shell.cms

shd:
	sxcmc --outdir=shell --verbose ../src/shell/shell.cms


prc:
	sxcmc --outdir=proc --verbose --config=release ../src/proc/proc.cms

prcd:
	sxcmc --outdir=proc --verbose ../src/proc/proc.cms

cu:
	sxcmc --outdir=coreutil --verbose --config=release ../src/coreutil/coreutil.cms

cud:
	sxcmc --outdir=coreutil --verbose ../src/coreutil/coreutil.cms

sys:
	sxcmc --outdir=system --verbose --config=release ../src/system/System.cms
	
sysd:
	sxcmc --outdir=system --verbose ../src/system/System.cms

mnm:
	cmsxas --verbose --outdir=main/lib ../src/main/Main.s

install:
	@echo off
	xcopy /Y /D coreutil\cat\bin\release ..\bin
	xcopy /Y /D coreutil\chmod\bin\release ..\bin
	xcopy /Y /D coreutil\cp\bin\release ..\bin
	xcopy /Y /D coreutil\echo\bin\release ..\bin
	xcopy /Y /D coreutil\fc\bin\release ..\bin
	xcopy /Y /D coreutil\grep\bin\release ..\bin
	xcopy /Y /D coreutil\hexdump\bin\release ..\bin
	xcopy /Y /D coreutil\ls\bin\release ..\bin
	xcopy /Y /D coreutil\mkdir\bin\release ..\bin
	xcopy /Y /D coreutil\mv\bin\release ..\bin
	xcopy /Y /D coreutil\passwd\bin\release ..\bin
	xcopy /Y /D coreutil\pwd\bin\release ..\bin
	xcopy /Y /D coreutil\rm\bin\release ..\bin
	xcopy /Y /D coreutil\rmdir\bin\release ..\bin
	xcopy /Y /D coreutil\tee\bin\release ..\bin
	xcopy /Y /D coreutil\time\bin\release ..\bin
	xcopy /Y /D coreutil\umask\bin\release ..\bin
	xcopy /Y /D coreutil\wc\bin\release ..\bin
	xcopy /Y /D coreutil\whoami\bin\release ..\bin
	xcopy /Y /D shell\cmsh\bin\release ..\bin

installd:
	@echo off
	xcopy /Y /D coreutil\cat\bin\debug ..\bind
	xcopy /Y /D coreutil\chmod\bin\debug ..\bind
	xcopy /Y /D coreutil\cp\bin\debug ..\bind
	xcopy /Y /D coreutil\echo\bin\debug ..\bind
	xcopy /Y /D coreutil\fc\bin\debug ..\bind
	xcopy /Y /D coreutil\grep\bin\debug ..\bind
	xcopy /Y /D coreutil\hexdump\bin\debug ..\bind
	xcopy /Y /D coreutil\ls\bin\debug ..\bind
	xcopy /Y /D coreutil\mkdir\bin\debug ..\bind
	xcopy /Y /D coreutil\mv\bin\debug ..\bind
	xcopy /Y /D coreutil\passwd\bin\debug ..\bind
	xcopy /Y /D coreutil\pwd\bin\debug ..\bind
	xcopy /Y /D coreutil\rm\bin\debug ..\bind
	xcopy /Y /D coreutil\rmdir\bin\debug ..\bind
	xcopy /Y /D coreutil\tee\bin\debug ..\bind
	xcopy /Y /D coreutil\time\bin\debug ..\bind
	xcopy /Y /D coreutil\umask\bin\debug ..\bind
	xcopy /Y /D coreutil\wc\bin\debug ..\bind
	xcopy /Y /D coreutil\whoami\bin\debug ..\bind
	xcopy /Y /D shell\cmsh\bin\debug ..\bind
