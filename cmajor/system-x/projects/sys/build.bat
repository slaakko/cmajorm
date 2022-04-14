@echo off
sxcmc -u -v coreutil\coreutil.cms
sxcmc -u -v -c=release coreutil\coreutil.cms
sxcmc -u -v editor\editor.cms
sxcmc -u -v -c=release editor\editor.cms
sxcmc -u -v shell\sh.cmp
sxcmc -u -v -c=release shell\sh.cmp
sxcmc -u -v tab\tab.cmp
sxcmc -u -v -c=release tab\tab.cmp
sxcmc -u -v sx\sx.cmp
sxcmc -u -v -c=release sx\sx.cmp
