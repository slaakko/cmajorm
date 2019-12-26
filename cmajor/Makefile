include Makefile.common

.PHONY : build install clean

all: build

build:
	$(MAKE) -C soulng
	$(MAKE) -C sngxml
	$(MAKE) -C sngcm
	$(MAKE) -C common
	$(MAKE) -C ir
	$(MAKE) -C cmllvm
	$(MAKE) -C symbols
	$(MAKE) -C cmmid
	$(MAKE) -C binder
	$(MAKE) -C codegenbase
	$(MAKE) -C codegenllvm
	$(MAKE) -C codegenwin
	$(MAKE) -C codegenlinux
	$(MAKE) -C codegen
	$(MAKE) -C cmdoclib
	$(MAKE) -C ast2dom
	$(MAKE) -C bdt2dom
	$(MAKE) -C build
	$(MAKE) -C compiler
	$(MAKE) -C cm2html
	$(MAKE) -C cmdoc
	$(MAKE) -C cmmdump
	$(MAKE) -C cmfileredirector
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
	cp lib/$(cmrtsoname).so.3.3.0 $(prefix)/lib
	cp lib/libgmpintf.a $(prefix)/lib
	ln -f $(prefix)/lib/$(cmrtsoname).so.3.3.0 $(prefix)/lib/$(cmrtsoname).so.3
	ln -f $(prefix)/lib/$(cmrtsoname).so.3.3.0 $(prefix)/lib/$(cmrtsoname).so

sys:
	cmc --verbose system/System.cms
	cmc --verbose --config=release system/System.cms

clean:
	$(MAKE) -C soulng clean
	$(MAKE) -C sngxml clean
	$(MAKE) -C sngcm clean
	$(MAKE) -C common clean
	$(MAKE) -C ir clean
	$(MAKE) -C cmllvm clean
	$(MAKE) -C symbols clean
	$(MAKE) -C cmmid clean
	$(MAKE) -C binder clean
	$(MAKE) -C codegenbase clean
	$(MAKE) -C codegenllvm clean
	$(MAKE) -C codegenwin clean
	$(MAKE) -C codegenlinux clean
	$(MAKE) -C codegen clean
	$(MAKE) -C cmdoclib clean
	$(MAKE) -C ast2dom clean
	$(MAKE) -C bdt2dom clean
	$(MAKE) -C build clean
	$(MAKE) -C compiler clean
	$(MAKE) -C cm2html clean
	$(MAKE) -C cmdoc clean
	$(MAKE) -C cmmdump clean
	$(MAKE) -C cmfileredirector clean
	$(MAKE) -C eh clean
	$(MAKE) -C rt clean
