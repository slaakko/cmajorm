@echo off
cmsxas --verbose --outdir=main/lib ../src/main/Main.s
sxcmc --outdir=system --verbose --config=release ../src/system/System.cms
sxcmc --outdir=coreutil --verbose --config=release ../src/coreutil/coreutil.cms
sxcmc --outdir=proc --verbose --config=release ../src/proc/proc.cms
sxcmc --outdir=shell --verbose --config=release ../src/shell/shell.cms
