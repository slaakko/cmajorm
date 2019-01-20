include ../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../lib/libutil.a ../lib/libcodedom.a ../lib/libparsing.a ../lib/libcode.a ../lib/libsyntax.a

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
    exename=cmpgd
else
    exename=cmpg
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) $(OBJECTS) -lsyntax -lcode -lparsing -lcodedom -lutil \
-lboost_filesystem -lboost_iostreams -lboost_system

clean:
	rm -f *.o
	rm -f ../bin/$(exename)

