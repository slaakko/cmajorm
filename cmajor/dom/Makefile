include ../Makefile.common

OBJECTS = CharacterData.o Document.o DocumentFragment.o Element.o Exception.o Node.o Parser.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libdom.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libdom.a

