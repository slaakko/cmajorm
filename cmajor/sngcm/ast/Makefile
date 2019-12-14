include ../../Makefile.common

OBJECTS = AstReader.o AstWriter.o Attribute.o BasicType.o Class.o Clone.o CompileUnit.o Concept.o Constant.o Delegate.o Enumeration.o Expression.o Function.o \
GlobalVariable.o Identifier.o InitDone.o Interface.o Literal.o Namespace.o Node.o Parameter.o Project.o Solution.o SourceToken.o Specifier.o Statement.o \
SystemFileIndex.o Template.o TypeExpr.o Typedef.o Visitor.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../../lib/libcmajorast.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../../lib/libcmajorast.a
