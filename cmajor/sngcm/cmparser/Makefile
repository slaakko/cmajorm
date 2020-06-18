include ../Makefile.common

OBJECTS = Attribute.o BasicType.o Class.o CompileUnit.o Concept.o Constant.o Delegate.o Enumeration.o Expression.o Function.o GlobalVariable.o Identifier.o Interface.o \
LexerFileTokenParser.o Literal.o Parameter.o ParserFileTokenParser.o ParsingContext.o ProjectFile.o SolutionFile.o SourceTokenParser.o Specifier.o Statement.o Template.o TypeExpr.o Typedef.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngcmcmparser.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngcmcmparser.a
