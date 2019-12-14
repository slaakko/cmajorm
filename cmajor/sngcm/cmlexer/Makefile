include ../../Makefile.common

OBJECTS = CmajorClassMap.o CmajorKeywords.o CmajorLexer.o CmajorTokens.o ContainerFileClassMap.o ContainerFileKeywords.o ContainerFileLexer.o ContainerFileTokens.o \
TokenValueParsers.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../../lib/libcmajorlexer.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../../lib/libcmajorlexer.a
