include ../Makefile.common

OBJECTS = main.o

LIBRARIES = ../lib/libutil.a ../lib/libcodedom.a ../lib/libparsing.a ../lib/libxpath.a ../lib/libdom.a ../lib/libxml.a

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
    exename=xpqd
else
    exename=xpq
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) $(OBJECTS) -lxpath -ldom -lxml -lparsing -lcodedom -lutil \
-lboost_filesystem -lboost_iostreams -lboost_system

clean:
	rm -f *.o
	rm -f ../bin/$(exename)

