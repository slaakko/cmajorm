include ../../Makefile.common

LIBRARY_NAME=libpdcursesd.a

LIBRARY_PREFIX=..\..\lib

all: $(PLATFORM)

windows: $(LIBRARY_PREFIX)/$(LIBRARY_NAME)

linux:
	# nothing to do for linux
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME):
	$(MAKE) -C ../../../../../system/ext/pdcurs36/wincon clean
	$(MAKE) -C ../../../../../system/ext/pdcurs36/wincon DEBUG=Y WIDE=Y UTF8=Y
	$(CP) ..\..\..\..\..\system\ext\pdcurs36\wincon\pdcurses.a $(LIBRARY_PREFIX)\$(LIBRARY_NAME)
