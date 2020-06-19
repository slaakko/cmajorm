include Makefile.common

all:
	$(MAKE) -C debug
	$(MAKE) -C release
	
clean:
	$(MAKE) -C debug clean
	$(MAKE) -C release clean
	
install: 
	$(CP) lib/*.a ../../../lib/clang
