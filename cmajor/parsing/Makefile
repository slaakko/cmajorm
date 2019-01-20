include ../Makefile.common

OBJECTS = Action.o Composite.o Exception.o Grammar.o InitDone.o Keyword.o Linking.o Namespace.o Nonterminal.o Parser.o ParsingData.o ParsingDomain.o \
ParsingObject.o Primitive.o Rule.o Scanner.o Scope.o StdLib.o Utility.o XmlLog.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libparsing.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libparsing.a

