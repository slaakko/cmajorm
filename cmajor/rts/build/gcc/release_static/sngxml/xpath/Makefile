include ../../../Makefile.common

OBJECTS = InitDone.o XPathApi.o XPathClassMap.o XPathContext.o XPathDebug.o XPathEvaluate.o XPathExpr.o XPathFunction.o XPathKeywords.o XPathLexer.o XPathObject.o \
XPathParser.o XPathTokenValueParsers.o XPathTokens.o

LIBRARY_NAME=libcmsngxmlxpath.a

SOURCE_PREFIX=../../../../../../sngxml/xpath

LIBRARY_PREFIX=../../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -DNDEBUG -O2

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
