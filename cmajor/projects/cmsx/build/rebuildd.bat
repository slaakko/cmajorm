@echo off
cmsxas --verbose --outdir=main/lib ../src/main/Main.s
sxcmc --rebuild --verbose --outdir=system ../src/system/System.cms
sxcmc --rebuild --verbose --outdir=coreutil ../src/coreutil/coreutil.cms
sxcmc --rebuild --verbose --outdir=proc ../src/proc/proc.cms
sxcmc --rebuild --verbose --outdir=shell ../src/shell/shell.cms
