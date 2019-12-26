include ../Makefile.common

OBJECTS = Match.o Range.o Value.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngparser.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngparser.a
