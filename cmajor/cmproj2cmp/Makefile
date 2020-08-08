include ../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../lib/libutil.a ../lib/libcodedom.a ../lib/libparsing.a ../lib/libxpath.a ../lib/libdom.a ../lib/libxml.a \
../lib/libast.a ../lib/libsymbols.a ../lib/libparser.a ../lib/libbuild.a ../lib/libbinder.a ../lib/libemitter.a \
../lib/libir.a ../lib/libcmproj.a

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
	exename=cmproj2cmpd
else
	exename=cmproj2cmp
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) $(OBJECTS)  -lcmproj -lbuild -lbinder -lemitter -lir -lsymbols -lparser -last \
-lxpath -ldom -lxml -lparsing -lcodedom -lutil \
-lboost_filesystem -lboost_iostreams -lboost_system -ldl -ltinfo -lz

clean:
	rm -f *.o
	rm -f ../bin/$(exename)

