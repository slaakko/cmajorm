include ../Makefile.common

OBJECTS = CmajorNothrowClassMap.o CmajorNothrowKeywords.o CmajorNothrowLexer.o CmajorNothrowTokens.o NothrowTokenValueParsers.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngcmcmnothrowlexer.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngcmcmnothrowlexer.a
