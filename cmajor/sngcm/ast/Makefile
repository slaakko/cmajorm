include ../Makefile.common

OBJECTS = AstReader.o AstWriter.o Attribute.o BasicType.o Class.o Clone.o Comment.o CompileUnit.o Concept.o Constant.o Delegate.o Enumeration.o Expression.o Function.o \
GlobalVariable.o Identifier.o InitDone.o Interface.o Literal.o Merge.o Namespace.o Node.o NodeList.o Parameter.o Project.o Solution.o SourceToken.o SourceWriter.o Specifier.o Statement.o \
SystemFileIndex.o Template.o TypeExpr.o Typedef.o Visitor.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngcmast.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngcmast.a
