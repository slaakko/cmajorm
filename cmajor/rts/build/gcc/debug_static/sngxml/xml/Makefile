include ../../../Makefile.common

OBJECTS = XmlApi.o XmlContentHandler.o XmlParser.o XmlParserInterface.o XmlProcessor.o

LIBRARY_NAME=libcmsngxmlxmld.a

SOURCE_PREFIX=../../../../../../sngxml/xml

LIBRARY_PREFIX=../../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -g -O0

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
