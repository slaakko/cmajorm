include ../../../Makefile.common

OBJECTS = AstReader.o AstWriter.o Attribute.o BasicType.o Class.o Clone.o \
Comment.o CompileUnit.o Concept.o Constant.o Delegate.o Enumeration.o \
Expression.o Function.o GlobalVariable.o Identifier.o Interface.o \
Literal.o Merge.o Namespace.o Node.o NodeList.o Parameter.o \
Project.o Solution.o SourceToken.o SourceWriter.o Specifier.o \
Statement.o SystemFileIndex.o Template.o Typedef.o TypeExpr.o \
Visitor.o

LIBRARY_NAME=libcmsngcmastd.a

SOURCE_PREFIX=../../../../../../sngcm/ast

LIBRARY_PREFIX=../../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -I$(SOURCE_PREFIX)/../../.. -g -O0

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
