include ../Makefile.common

OBJECTS = XmlApi.o XmlContentHandler.o XmlParser.o XmlParserInterface.o XmlProcessor.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngxmlxml.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngxmlxml.a
