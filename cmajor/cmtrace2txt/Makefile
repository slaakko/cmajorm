include ../Makefile.common

OBJECTS = Main.o

LIBRARIES = ../lib/libcmsngutil.a ../lib/libcmsnglexer.a ../lib/libcmsngparser.a ../lib/libcmsngxmlxml.a ../lib/libcmsngxmldom.a ../lib/libcmsngxmlxpath.a ../lib/libserialization.a \
../lib/libcmsngcmast.a ../lib/libcmsngcmcmlexer.a ../lib/libcmsngcmcmparser.a ../lib/libcmsngcmcmnothrowlexer.a ../lib/libcmsngcmcmnothrowparser.a \
../lib/libcmcommon.a ../lib/libcmir.a ../lib/libcmllvm.a ../lib/libcmsymbols.a \
../lib/libcmmid.a ../lib/libcmbinder.a ../lib/libcmcodegenbase.a ../lib/libcmcodegenllvm.a ../lib/libcmcodegenwin.a ../lib/libcmcodegenlinux.a ../lib/libcmcodegen.a \
../lib/libcmdoclib.a ../lib/libcmast2dom.a ../lib/libcmbdt2dom.a ../lib/libcmbuild.a ../lib/libcmtoolchain.a ../lib/libcmsngjsonjson.a ../lib/libcmcodegencpp.a \
../lib/libcmcppbe.a ../lib/libcmcppi.a ../lib/libcmdebug.a ../lib/libcmmsg.a

LLVMLIBS = $(shell llvm-config --libs)

CXXFLAGS += -L$(shell llvm-config --libdir)

CXXFLAGS += -L../lib

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
exename=cmtrace2txtd
else
exename=cmtrace2txt
endif

../bin/$(exename): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -o ../bin/$(exename) $(OBJECTS) \
-lcmbuild -lcmsngjsonjson -lcmtoolchain -lcmast2dom -lcmbdt2dom -lcmdoclib -lcmcodegen \
-lcmcodegenlinux -lcmcodegenwin -lcmcodegenllvm -lcmcodegenbase -lcmcodegencpp  -lcmbinder -lcmmid -lcmsymbols -lcmdebug -lcmmsg \
-lcmllvm -lcmcppbe -lcmcppi -lcmir -lcmcommon -lcmtrace -lcmsngrex -lcmsngcmcmparser -lcmsngcmcmlexer -lcmsngcmcmnothrowparser -lcmsngcmcmnothrowlexer -lcmsngcmast -lcmtoolchain \
-lserialization -lcmsngxmlxpath -lcmsngxmldom -lcmsngxmlxml -lcmsngparser -lcmsnglexer -lcmsngutil \
$(LLVMLIBS) -lboost_filesystem -lboost_iostreams -lboost_system -ldl -ltinfo -lz

clean:
	rm -f *.o
	rm -f ../bin/$(exename)
