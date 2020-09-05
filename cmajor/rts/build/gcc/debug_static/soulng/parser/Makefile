include ../../../Makefile.common

OBJECTS = Match.o Range.o Value.o

LIBRARY_NAME=libcmsngparserd.a

SOURCE_PREFIX=../../../../../../soulng/parser

LIBRARY_PREFIX=../../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -g -O0

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
