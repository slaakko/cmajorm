include Makefile.common

ifeq ($(PLATFORM),windows)

all:
	$(MAKE) -C debug_static
	$(MAKE) -C debug_shared
	$(MAKE) -C release_static
	$(MAKE) -C release_shared
	
clean:
	$(MAKE) -C debug_static clean
	$(MAKE) -C debug_shared clean
	$(MAKE) -C release_static clean
	$(MAKE) -C release_shared clean
	
else

all:
	$(MAKE) -C debug_static
	$(MAKE) -C release_static

clean:
	$(MAKE) -C debug_static clean
	$(MAKE) -C release_static clean

endif

install: $(PLATFORM)

windows:
	$(CP) lib\*.a ..\..\..\lib\gcc
	$(CP) bin\* ..\..\..\bin
	
linux:
	$(CP) lib/*.a ../../../lib/gcc

