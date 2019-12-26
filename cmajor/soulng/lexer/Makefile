include ../Makefile.common

OBJECTS = Keyword.o Lexeme.o Lexer.o ParsingException.o ParsingLog.o Span.o Token.o TrivialLexer.o XmlParsingLog.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsnglexer.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsnglexer.a
