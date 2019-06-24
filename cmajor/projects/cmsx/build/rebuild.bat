@echo off
cmsxas --verbose --outdir=main/lib ../src/main/Main.s
sxcmc --rebuild --verbose --outdir=system --config=release ../src/system/System.cms
sxcmc --rebuild --verbose --outdir=coreutil --config=release ../src/coreutil/coreutil.cms
sxcmc --rebuild --verbose --outdir=proc --config=release ../src/proc/proc.cms
sxcmc --rebuild --verbose --outdir=shell --config=release ../src/shell/shell.cms
