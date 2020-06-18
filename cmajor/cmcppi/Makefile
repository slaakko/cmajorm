include ../Makefile.common

OBJECTS = BasicBlock.o CompileUnit.o Context.o Data.o Function.o Instruction.o Type.o Value.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmcppi.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmcppi.a

