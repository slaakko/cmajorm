include ../../../Makefile.common

OBJECTS = CharacterData.o Document.o DocumentFragment.o DomApi.o Element.o Exception.o Node.o Parser.o

LIBRARY_NAME=libcmsngxmldomd.a

SOURCE_PREFIX=../../../../../../sngxml/dom

LIBRARY_PREFIX=../../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -g -O0

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
