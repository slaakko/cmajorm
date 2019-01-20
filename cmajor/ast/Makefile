include ../Makefile.common

OBJECTS = AstReader.o AstWriter.o Attribute.o BasicType.o Class.o Clone.o CompileUnit.o Concept.o Constant.o Delegate.o Enumeration.o Expression.o \
Function.o Identifier.o InitDone.o Interface.o Literal.o Namespace.o Node.o Parameter.o Project.o Solution.o SourceToken.o Specifier.o Statement.o \
SystemFileIndex.o Template.o Typedef.o TypeExpr.o Visitor.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libast.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libast.a

