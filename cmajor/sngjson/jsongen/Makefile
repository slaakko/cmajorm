include ../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../../lib/libcmsngutil.a ../../lib/libcmsnglexer.a ../../lib/libcmsngparser.a ../../lib/libcmsngxmlxml.a ../../lib/libcmsngxmldom.a ../../lib/libcmsngxmlxpath.a \
../../lib/libcmsngcmast.a ../../lib/libcmsngcmcmlexer.a ../../lib/libcmsngcmcmparser.a ../../lib/libcmsngjsonjson.a

CXXFLAGS += -L../../lib

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
exename=jsongend
else
exename=jsongen
endif

../../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../../bin/$(exename) $(OBJECTS) -lcmsngjsonjson -lcmsngcmcmparser -lcmsngcmcmlexer -lcmsngcmast \
-lcmsngxmlxpath -lcmsngxmldom -lcmsngxmlxml -lcmsngparser -lcmsnglexer -lcmsngutil \
-lboost_filesystem -lboost_iostreams -lboost_system -ldl -ltinfo -lz

clean:
	rm -f *.o
	rm -f ../../bin/$(exename)
