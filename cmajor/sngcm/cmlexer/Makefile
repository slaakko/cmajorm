include ../Makefile.common

OBJECTS = CmajorClassMap.o CmajorKeywords.o CmajorLexer.o CmajorTokens.o ContainerFileClassMap.o ContainerFileKeywords.o ContainerFileLexer.o ContainerFileTokens.o \
LexerFileTokenLexer.o LexerFileTokenLexerClassMap.o LexerFileTokenLexerKeywords.o LexerFileTokenLexerTokens.o ParserFileTokenLexer.o ParserFileTokenLexerClassMap.o \
ParserFileTokenLexerKeywords.o ParserFileTokenLexerTokens.o \
TokenLexer.o TokenLexerClassMap.o TokenLexerKeywords.o TokenLexerTokens.o TokenValueParsers.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngcmcmlexer.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngcmcmlexer.a
