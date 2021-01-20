include ../Makefile.common

OBJECTS = Trace.o TraceClassMap.o TraceExpr.o TraceKeywords.o TraceLexer.o TraceNode.o TraceParser.o TraceTokens.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../lib/libcmtrace.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmtrace.a
