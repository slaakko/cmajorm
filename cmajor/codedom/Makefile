include ../Makefile.common

OBJECTS = Declaration.o Expression.o InitDone.o Literal.o Object.o Operator.o Statement.o Type.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcodedom.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcodedom.a

