include ../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../lib/libutil.a ../lib/libcodedom.a ../lib/libparsing.a ../lib/libxpath.a ../lib/libdom.a ../lib/libxml.a \
../lib/libast.a ../lib/libast2dom.a ../lib/libbdt2dom.a ../lib/libsymbols.a ../lib/libparser.a ../lib/libbinder.a ../lib/libbuild.a ../lib/libemitter.a \
../lib/libir.a ../lib/libcmdoclib.a

LLVMLIBS = $(shell llvm-config --libs)

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
    exename=cmunitd
else
    exename=cmunit
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) $(OBJECTS) -lbuild -lemitter -lir -lbinder -lcmdoclib -lsymbols -lparser -last -last2dom -lbdt2dom \
-lxpath -ldom -lxml -lparsing -lcodedom -lutil \
$(LLVMLIBS) -lboost_filesystem -lboost_iostreams -lboost_system -ldl -ltinfo -lz

clean:
	rm -f *.o
	rm -f ../bin/$(exename)

