@echo off
sxcmc -u -v coreutil\coreutil.cms
sxcmc -u -v -c=release coreutil\coreutil.cms
sxcmc -u -v shell\sh.cmp
sxcmc -u -v -c=release shell\sh.cmp
