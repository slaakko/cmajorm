include ../Makefile.common

OBJECTS = JsonClassMap.o JsonExport.o JsonImport.o JsonLexer.o JsonParser.o JsonSchema.o JsonSchemaClassMap.o JsonSchemaLexer.o \
JsonSchemaParser.o JsonSchemaTokens.o JsonTokens.o TokenValueParsers.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngjsonjson.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngjsonjson.a
