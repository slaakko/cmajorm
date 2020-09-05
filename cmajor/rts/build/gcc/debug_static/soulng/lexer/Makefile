include ../../../Makefile.common

OBJECTS = Keyword.o Lexeme.o Lexer.o ParsingException.o ParsingLog.o Span.o Token.o TrivialLexer.o XmlParsingLog.o

LIBRARY_NAME=libcmsnglexerd.a

SOURCE_PREFIX=../../../../../../soulng/lexer

LIBRARY_PREFIX=../../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -g -O0

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
