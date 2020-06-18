include ../Makefile.common

OBJECTS = Constraint.o Diagram.o Doc.o File.o Global.o InitDone.o Input.o ParserDoc.o SourceCodePrinter.o Style.o SymbolTableXml.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmdoclib.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmdoclib.a

