include ../Makefile.common

OBJECTS = Match.o Range.o Value.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libsngparser.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libsngparser.a
