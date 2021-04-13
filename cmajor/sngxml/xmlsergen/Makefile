include ../../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../../lib/libserialization.a ../../lib/libcmsngutil.a ../../lib/libcmsnglexer.a ../../lib/libcmsngparser.a ../../lib/libcmsngxmlxml.a \
../../lib/libcmsngxmldom.a ../../lib/libcmsngxmlxpath.a

CXXFLAGS += -L../../lib

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
exename=xmlsergend
else
exename=xmlsergen
endif

../../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../../bin/$(exename) $(OBJECTS) \
-lserialization -lcmsngxmlxpath -lcmsngxmldom -lcmsngxmlxml -lcmsngparser -lcmsnglexer -lcmsngutil \
-lboost_filesystem -lboost_iostreams -lboost_system -ldl -ltinfo -lz

clean:
	rm -f *.o
	rm -f ../../bin/$(exename)

