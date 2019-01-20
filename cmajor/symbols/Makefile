include ../Makefile.common

OBJECTS = ArrayTypeSymbol.o BasicTypeOperation.o BasicTypeSymbol.o ClassTemplateSpecializationSymbol.o ClassTypeSymbol.o ConceptSymbol.o \
ConstantSymbol.o ContainerSymbol.o ConversionTable.o DebugFlags.o DelegateSymbol.o DerivedTypeSymbol.o EnumSymbol.o Exception.o FunctionSymbol.o \
GlobalFlags.o InitDone.o InterfaceTypeSymbol.o Meta.o Module.o ModuleCache.o NamespaceSymbol.o Operation.o Scope.o SymbolCollector.o Symbol.o \
SymbolCreatorVisitor.o SymbolReader.o SymbolTable.o SymbolWriter.o TemplateSymbol.o TypedefSymbol.o TypeMap.o TypeSymbol.o Value.o VariableSymbol.o \
Warning.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libsymbols.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libsymbols.a

