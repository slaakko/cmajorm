@echo off
sxcmc --rebuild --verbose --outdir=system ../src/system/System.cms
sxcmc --rebuild --verbose --outdir=coreutil ../src/coreutil/coreutil.cms
sxcmc --rebuild --verbose --outdir=init ../src/init/init.cms
