include ../Makefile.common

OBJECTS = InitDone.o XPathContext.o XPath.o XPathDebug.o XPathEvaluate.o XPathExpr.o XPathFunction.o XPathObject.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libxpath.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libxpath.a

