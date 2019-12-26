include ../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../lib/libcmsngutil.a

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
exename=cmfileredirectord
else
exename=cmfileredirector
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) $(OBJECTS) -lcmsngutil -lboost_filesystem -lboost_iostreams -lboost_system -ldl -ltinfo -lz

clean:
	rm -f *.o
	rm -f ../bin/$(exename)

