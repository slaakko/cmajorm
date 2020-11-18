include ../../../Makefile.common

OBJECTS = CmajorClassMap.o CmajorKeywords.o CmajorLexer.o CmajorTokens.o \
ContainerFileClassMap.o ContainerFileKeywords.o ContainerFileLexer.o \
ContainerFileTokens.o LexerFileTokenLexer.o LexerFileTokenLexerClassMap.o \
LexerFileTokenLexerKeywords.o LexerFileTokenLexerTokens.o ParserFileTokenLexer.o \
ParserFileTokenLexerClassMap.o ParserFileTokenLexerKeywords.o \
ParserFileTokenLexerTokens.o TokenLexer.o TokenLexerClassMap.o TokenLexerKeywords.o \
TokenLexerTokens.o TokenValueParsers.o

LIBRARY_NAME=libcmsngcmlexerd.a

SOURCE_PREFIX=../../../../../../sngcm/cmlexer

LIBRARY_PREFIX=../../../lib

all: $(PLATFORM)

windows: $(LIBRARY_PREFIX)/$(LIBRARY_NAME)

linux:
	# nothing to do for linux

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -I$(SOURCE_PREFIX)/../../.. -g -O0

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
