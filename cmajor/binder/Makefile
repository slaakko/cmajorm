include ../Makefile.common

OBJECTS = Access.o AttributeBinder.o BoundClass.o BoundCompileUnit.o BoundConstraint.o BoundEnum.o BoundExpression.o BoundFunction.o BoundNamespace.o BoundNode.o \
BoundNodeVisitor.o BoundStatement.o ClassTemplateRepository.o Concept.o ConceptRepository.o ConstExprFunctionRepository.o ConstantArrayRepository.o ConstantStructureRepository.o \
ControlFlowAnalyzer.o Evaluator.o ExpressionBinder.o FunctionTemplateRepository.o InlineFunctionRepository.o \
JsonAttributeProcessor.o OperationRepository.o OverloadResolution.o StatementBinder.o StringRepository.o TypeBinder.o TypeResolver.o \
UuidRepository.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmbinder.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmbinder.a

