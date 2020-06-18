include ../Makefile.common

OBJECTS = InitDone.o XPathApi.o XPathClassMap.o XPathContext.o XPathDebug.o XPathEvaluate.o XPathExpr.o XPathFunction.o XPathKeywords.o XPathLexer.o XPathObject.o \
XPathParser.o XPathTokenValueParsers.o XPathTokens.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngxmlxpath.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngxmlxpath.a
