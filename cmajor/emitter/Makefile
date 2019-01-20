include ../Makefile.common

OBJECTS = BasicEmitter.o Emitter.o EmittingContext.o EmittingContextImpl.o LinuxEmitter.o WindowsEmitter.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libemitter.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libemitter.a

