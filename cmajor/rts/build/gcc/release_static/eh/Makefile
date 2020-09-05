include ../../Makefile.common

OBJECTS = Exception.o

LIBRARY_NAME=libcmeh.a

SOURCE_PREFIX=../../../../../eh

LIBRARY_PREFIX=../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -I$(SOURCE_PREFIX)/.. -DNDEBUG -O2

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
