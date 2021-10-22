include Makefile.common

OBJECTS = InitDone.o \
XmlBundle.o XmlClassRegistry.o XmlContainer.o \
XmlExport.o XmlImport.o XmlPtr.o XmlSerApi.o XmlSerAst.o XmlSerClassMap.o XmlSerGen.o \
XmlSerializable.o XmlSerializationContext.o \
XmlSerKeywords.o XmlSerLexer.o XmlSerParser.o XmlSerRules.o XmlSerScan.o XmlSerTokens.o XmlSerVisitor.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../../lib/libserialization.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libserialization.a
