include ../Makefile.common

ifeq ($(config),debug)
CXXFLAGS += -I$(shell llvm-config --includedir) -I../.. -g -O0
else
CXXFLAGS += -I$(shell llvm-config --includedir) -I../.. -DNDEBUG -g -O3
endif

OBJECTS = Emitter.o EmittingContext.o Interface.o ValueStack.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmllvm.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmllvm.a
