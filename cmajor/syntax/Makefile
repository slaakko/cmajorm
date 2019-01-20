include ../Makefile.common

OBJECTS = CodeGenerator.o CodeGeneratorVisitor.o Composite.o Element.o Generator.o Grammar.o Library.o LibraryFile.o ParserFileContent.o \
ParserFile.o Primary.o Primitive.o Project.o ProjectFile.o Rule.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libsyntax.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libsyntax.a

