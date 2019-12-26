include ../Makefile.common

OBJECTS = Emitter.o EmittingContext.o GenObject.o ValueStack.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmir.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmir.a
