include Makefile.common

.PHONY : build install clean

all: build

build:
	$(MAKE) -C util
	$(MAKE) -C codedom
	$(MAKE) -C parsing
	$(MAKE) -C code
	$(MAKE) -C syntax
	$(MAKE) -C generator
	$(MAKE) -C xml
	$(MAKE) -C dom
	$(MAKE) -C xpath
	$(MAKE) -C xpq
	$(MAKE) -C ast
	$(MAKE) -C ast2dom
	$(MAKE) -C bdt2dom
	$(MAKE) -C parser
	$(MAKE) -C symbols
	$(MAKE) -C cmproj
	$(MAKE) -C ir
	$(MAKE) -C emitter
	$(MAKE) -C binder
	$(MAKE) -C build
	$(MAKE) -C cmdoclib
	$(MAKE) -C compiler
	$(MAKE) -C cm2html
	$(MAKE) -C cmdoc
	$(MAKE) -C cmmdump
	$(MAKE) -C cmp2cmproj
	$(MAKE) -C cms2sln
	$(MAKE) -C cmfileredirector
	$(MAKE) -C cmprof
	$(MAKE) -C cmunit
	$(MAKE) -C system/ext/gmp
	$(MAKE) -C eh
	$(MAKE) -C rt

prefix := /usr/local

ifeq ($(config),debug)
    cmrtsoname=libcmrtd
else
    cmrtsoname=libcmrt
endif

install:
	mkdir -p $(prefix)/bin
	cp bin/* $(prefix)/bin
	mkdir -p $(prefix)/lib
	cp lib/$(cmrtsoname).so.3.2.0 $(prefix)/lib
	cp lib/libgmpintf.a $(prefix)/lib
	ln -f $(prefix)/lib/$(cmrtsoname).so.3.2.0 $(prefix)/lib/$(cmrtsoname).so.3
	ln -f $(prefix)/lib/$(cmrtsoname).so.3.2.0 $(prefix)/lib/$(cmrtsoname).so

sys:
	cmc --verbose system/System.cms
	cmc --verbose --config=release system/System.cms

clean:
	$(MAKE) -C util clean
	$(MAKE) -C codedom clean
	$(MAKE) -C parsing clean
	$(MAKE) -C code clean
	$(MAKE) -C syntax clean
	$(MAKE) -C generator clean
	$(MAKE) -C xml clean
	$(MAKE) -C dom clean
	$(MAKE) -C xpath clean
	$(MAKE) -C xpq clean
	$(MAKE) -C ast clean
	$(MAKE) -C ast2dom clean
	$(MAKE) -C bdt2dom clean
	$(MAKE) -C parser clean
	$(MAKE) -C symbols clean
	$(MAKE) -C cmproj clean
	$(MAKE) -C ir clean
	$(MAKE) -C emitter clean
	$(MAKE) -C binder clean
	$(MAKE) -C build clean
	$(MAKE) -C cmdoclib clean
	$(MAKE) -C compiler clean
	$(MAKE) -C cm2html clean
	$(MAKE) -C cmdoc clean
	$(MAKE) -C cmmdump clean
	$(MAKE) -C cmp2cmproj clean
	$(MAKE) -C cms2sln clean
	$(MAKE) -C cmfileredirector clean
	$(MAKE) -C cmprof clean
	$(MAKE) -C cmunit clean
	$(MAKE) -C system/ext/gmp clean
	$(MAKE) -C eh clean
	$(MAKE) -C rt clean

