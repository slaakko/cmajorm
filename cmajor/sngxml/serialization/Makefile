include Makefile.common

OBJECTS = InitDone.o XmlClassRegistry.o XmlExport.o XmlImport.o XmlSerApi.o XmlSerAst.o XmlSerClassMap.o XmlSerGen.o \
XmlSerKeywords.o XmlSerLexer.o XmlSerParser.o XmlSerTokens.o XmlSerVisitor.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../../lib/libserialization.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libserialization.a
