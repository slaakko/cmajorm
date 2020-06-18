include ../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../lib/libcmsngutil.a ../lib/libcmsnglexer.a ../lib/libcmsngparser.a ../lib/libcmsngxmlxml.a ../lib/libcmsngxmldom.a ../lib/libcmsngxmlxpath.a \
../lib/libcmsngcmast.a ../lib/libcmsngcmcmlexer.a ../lib/libcmsngcmcmparser.a ../lib/libcmcommon.a ../lib/libcmir.a ../lib/libcmsymbols.a \
../lib/libcmmid.a ../lib/libcmbinder.a ../lib/libcmcodegenbase.a ../lib/libcmcodegen.a \
../lib/libcmdoclib.a ../lib/libcmast2dom.a ../lib/libcmbdt2dom.a ../lib/libcmbuild.a ../lib/libcmtoolchain.a

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
exename=cppcmcd
else
exename=cppcmc
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) $(OBJECTS) -lcmbuild -lcmast2dom -lcmbdt2dom -lcmdoclib -lcmcodegen -lcmcodegenbase -lcmbinder -lcmmid -lcmsymbols \
-lcmir -lcmcommon -lcmsngcmcmparser -lcmsngcmcmlexer -lcmsngcmast -lcmtoolchain \
-lcmsngxmlxpath -lcmsngxmldom -lcmsngxmlxml -lcmsngparser -lcmsnglexer -lcmsngutil \
-lboost_filesystem -lboost_iostreams -lboost_system -ldl -ltinfo -lz

clean:
	rm -f *.o
	rm -f ../bin/$(exename)
