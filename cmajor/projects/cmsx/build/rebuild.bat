@echo off
cmsxas --verbose --outdir=main/lib ../src/main/Main.s
sxcmc --rebuild --verbose --outdir=system --config=release ../src/system/System.cms
sxcmc --rebuild --verbose --outdir=coreutil --config=release ../src/coreutil/coreutil.cms
sxcmc --rebuild --verbose --outdir=init --config=release ../src/init/init.cmp
