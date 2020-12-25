include ../Makefile.common

OBJECTS = Algorithm.o Context.o Match.o Nfa.o RexClassMap.o RexLexer.o RexParser.o RexTokens.o Symbol.o TokenValueParsers.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngrex.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngrex.a
