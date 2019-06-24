@echo off
cmsxas --verbose --outdir=main/lib ../src/main/Main.s
sxcmc --outdir=system --verbose ../src/system/System.cms
sxcmc --outdir=coreutil --verbose ../src/coreutil/coreutil.cms
sxcmc --outdir=proc --verbose ../src/proc/proc.cms
sxcmc --outdir=shell --verbose ../src/shell/shell.cms
