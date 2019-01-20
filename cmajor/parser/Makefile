include ../Makefile.common

OBJECTS = Attribute.o BasicType.o Class.o CommandLine.o CompileUnit.o Concept.o Constant.o Delegate.o Enumeration.o Expression.o FileRegistry.o \
Function.o Identifier.o Interface.o Json.o Keyword.o Literal.o Parameter.o ParsingContext.o Project.o Solution.o SourceToken.o Specifier.o \
Statement.o Template.o Typedef.o TypeExpr.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libparser.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libparser.a

