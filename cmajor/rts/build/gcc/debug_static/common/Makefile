include ../../Makefile.common

OBJECTS = Common.o

LIBRARY_NAME=libcmcommond.a

SOURCE_PREFIX=../../../../../common

LIBRARY_PREFIX=../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -I$(SOURCE_PREFIX)/.. -g -O0

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
