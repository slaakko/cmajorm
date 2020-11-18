include ../../../Makefile.common

OBJECTS = Attribute.o BasicType.o Class.o CommandLine.o CompileUnit.o Concept.o \
Constant.o Delegate.o Enumeration.o Expression.o Function.o GlobalVariable.o \
Identifier.o Interface.o LexerFileTokenParser.o Literal.o Parameter.o \
ParserFileTokenParser.o ParsingContext.o ProjectFile.o SolutionFile.o \
SourceTokenParser.o Specifier.o Statement.o Template.o Typedef.o \
TypeExpr.o

LIBRARY_NAME=libcmsngcmparser.a

SOURCE_PREFIX=../../../../../../sngcm/cmparser

LIBRARY_PREFIX=../../../lib

all: $(PLATFORM)

windows: $(LIBRARY_PREFIX)/$(LIBRARY_NAME)

linux:
	# nothing to do for linux

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -I$(SOURCE_PREFIX)/../../.. -DNDEBUG -O2

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
