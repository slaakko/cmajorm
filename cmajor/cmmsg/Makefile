include ../Makefile.common

OBJECTS = BuildServerMessage.o CmdbMessage.o CmdbMessageMap.o CodeCompletionServerMessage.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmmsg.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmmsg.a
