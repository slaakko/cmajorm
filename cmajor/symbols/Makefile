include ../Makefile.common

OBJECTS = ArrayTypeSymbol.o BasicTypeOperation.o BasicTypeSymbol.o ClassTemplateSpecializationSymbol.o ClassTypeSymbol.o ConceptSymbol.o \
ConstantSymbol.o ContainerSymbol.o ConversionTable.o DebugFlags.o DelegateSymbol.o DerivedTypeSymbol.o EditModuleCache.o \
EnumSymbol.o Exception.o FileIndex.o FunctionIndex.o FunctionSymbol.o \
GlobalFlags.o InitDone.o InterfaceTypeSymbol.o Meta.o Module.o ModuleCache.o NamespaceSymbol.o Operation.o Scope.o StringFunctions.o SourceFileModuleMap.o \
Sources.o Symbol.o SymbolCollector.o \
SymbolCreatorVisitor.o SymbolReader.o SymbolTable.o SymbolWriter.o TemplateSymbol.o Trace.o TypeMap.o TypeSymbol.o TypedefSymbol.o TypeIndex.o Value.o VariableSymbol.o KeywordSymbol.o \
Warning.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmsymbols.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmsymbols.a

