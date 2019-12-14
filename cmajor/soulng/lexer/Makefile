include ../Makefile.common

OBJECTS = Comment.o Keyword.o Lexeme.o Lexer.o ParsingLog.o Span.o Token.o TrivialLexer.o Value.o XmlParsingLog.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libsnglexer.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libsnglexer.a
