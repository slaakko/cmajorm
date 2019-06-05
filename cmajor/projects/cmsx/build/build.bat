@echo off
sxcmc --outdir=system --verbose ../src/system/System.cms
sxcmc --outdir=coreutil --verbose ../src/coreutil/coreutil.cms
sxcmc --outdir=init --verbose ../src/init/init.cms
