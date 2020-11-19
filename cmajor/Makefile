include Makefile.common

.PHONY : build install clean

all: build

build:
	$(MAKE) -C soulng
	$(MAKE) -C sngxml
	$(MAKE) -C sngcm
	$(MAKE) -C sngjson
	$(MAKE) -C common
	$(MAKE) -C ir
	$(MAKE) -C cmllvm
	$(MAKE) -C cmcppbe
	$(MAKE) -C cmcppi
	$(MAKE) -C cmtoolchain
	$(MAKE) -C cmtoolchainconfig
	$(MAKE) -C cmdebug
	$(MAKE) -C symbols
	$(MAKE) -C cmmid
	$(MAKE) -C binder
	$(MAKE) -C codegenbase
	$(MAKE) -C codegenllvm
	$(MAKE) -C codegenwin
	$(MAKE) -C codegenlinux
	$(MAKE) -C codegencpp
	$(MAKE) -C codegen
	$(MAKE) -C cmdoclib
	$(MAKE) -C ast2dom
	$(MAKE) -C bdt2dom
	$(MAKE) -C build
	$(MAKE) -C compiler
	$(MAKE) -C cmclient
	$(MAKE) -C cmserver
	$(MAKE) -C cppcmc
	$(MAKE) -C cmdb
	$(MAKE) -C cmdb2xml
	$(MAKE) -C cmupd
	$(MAKE) -C cm2html
	$(MAKE) -C cmdoc
	$(MAKE) -C cmmdump
	$(MAKE) -C cmfileredirector
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
	cp lib/$(cmrtsoname).so.3.7.0 $(prefix)/lib
	ln -f $(prefix)/lib/$(cmrtsoname).so.3.7.0 $(prefix)/lib/$(cmrtsoname).so.3
	ln -f $(prefix)/lib/$(cmrtsoname).so.3.7.0 $(prefix)/lib/$(cmrtsoname).so

sys:
	cmc --verbose system/platform/linux/System.cms
	cmc --verbose --config=release system/platform/linux/System.cms

SRCDIR = ~/cmajor-3.7.0-src
SRCPACKAGE = cmajor-3.7.0-src.tar.bz2
SRCDIRNAME = cmajor-3.7.0-src

srcpackage:
	mkdir -p $(SRCDIR)
	cd ~/cmajorm && git archive master | tar -x -C $(SRCDIR)
	rm -r $(SRCDIR)/devenv
	rm -r $(SRCDIR)/setup
	rm -r $(SRCDIR)/vs
	rm -r $(SRCDIR)/cmajor/vs
	rm $(SRCDIR)/cmajor/supd.xml
	mkdir -p $(SRCDIR)/cmajor/bin
	mkdir -p $(SRCDIR)/cmajor/lib
	mkdir -p $(SRCDIR)/cmajor/lib/gcc
	mkdir -p $(SRCDIR)/cmajor/rts/build/gcc/bin
	mkdir -p $(SRCDIR)/cmajor/rts/build/gcc/lib
	cp /mnt/c/work/cmajorm/cmajor/unicode/cmajor_ucd.bin $(SRCDIR)/cmajor/unicode
	cd ~ && tar cjf $(SRCPACKAGE) $(SRCDIRNAME)

BINDISTDIR = ~/cmajor-3.7.0
BINDISTPACKAGE = cmajor-3.7.0-ubuntu-20.04-x86_64-binaries.tar.bz2
BINDISTDIRNAME = cmajor-3.7.0

bindist:
	cmc --clean ~/cmajor-3.7.0-src/cmajor/system/platform/linux/System.cms
	cmc --clean -c=release ~/cmajor-3.7.0-src/cmajor/system/platform/linux/System.cms
	mkdir -p $(BINDISTDIR)
	mkdir -p $(BINDISTDIR)/bin
	mkdir -p $(BINDISTDIR)/config
	mkdir -p $(BINDISTDIR)/lib
	mkdir -p $(BINDISTDIR)/lib/gcc
	mkdir -p $(BINDISTDIR)/doc
	mkdir -p $(BINDISTDIR)/system
	mkdir -p $(BINDISTDIR)/unicode
	mkdir -p $(BINDISTDIR)/projects
	cp bin/* $(BINDISTDIR)/bin
	cp config/* $(BINDISTDIR)/config
	cp -r lib/* $(BINDISTDIR)/lib
	cp -r doc/* $(BINDISTDIR)/doc
	cp -r system/* $(BINDISTDIR)/system
	cp -r projects/* $(BINDISTDIR)/projects
	cp  unicode/cmajor_ucd.bin $(BINDISTDIR)/unicode
	mkdir -p $(BINDISTDIR)/cmajor/common
	rm -f common/*.o
	cp -r common/* $(BINDISTDIR)/cmajor/common
	mkdir -p $(BINDISTDIR)/cmajor/eh
	rm -f eh/*.o
	cp -r eh/* $(BINDISTDIR)/cmajor/eh
	mkdir -p $(BINDISTDIR)/cmajor/rts
	rm -f rts/*.o
	cp -r rts/* $(BINDISTDIR)/cmajor/rts
	mkdir -p $(BINDISTDIR)/cmajor/sngxml
	rm -f sngxml/dom/*.o
	rm -f sngxml/xml/*.o
	rm -f sngxml/xpath/*.o
	cp -r sngxml/* $(BINDISTDIR)/cmajor/sngxml
	mkdir -p $(BINDISTDIR)/cmajor/soulng
	rm -f soulng/util/*.o
	rm -f soulng/lexer/*.o
	rm -f soulng/parser/*.o
	cp -r soulng/* $(BINDISTDIR)/cmajor/soulng
	cd $(BINDISTDIR)/cmajor && ln -s ../system
	cd $(BINDISTDIR)/cmajor && ln -s ../lib
	cd ~ && tar cjf $(BINDISTPACKAGE) $(BINDISTDIRNAME)
	
clean:
	$(MAKE) -C soulng clean
	$(MAKE) -C sngxml clean
	$(MAKE) -C sngcm clean
	$(MAKE) -C sngjson clean
	$(MAKE) -C common clean
	$(MAKE) -C ir clean
	$(MAKE) -C cmllvm clean
	$(MAKE) -C cmcppbe clean
	$(MAKE) -C cmcppi clean
	$(MAKE) -C cmtoolchain clean
	$(MAKE) -C cmtoolchainconfig clean
	$(MAKE) -C cmdebug clean
	$(MAKE) -C symbols clean
	$(MAKE) -C cmmid clean
	$(MAKE) -C binder clean
	$(MAKE) -C codegenbase clean
	$(MAKE) -C codegenllvm clean
	$(MAKE) -C codegenwin clean
	$(MAKE) -C codegenlinux clean
	$(MAKE) -C codegencpp clean
	$(MAKE) -C codegen clean
	$(MAKE) -C cmdoclib clean
	$(MAKE) -C ast2dom clean
	$(MAKE) -C bdt2dom clean
	$(MAKE) -C build clean
	$(MAKE) -C compiler clean
	$(MAKE) -C cmclient clean
	$(MAKE) -C cmserver clean
	$(MAKE) -C cppcmc clean
	$(MAKE) -C cmdb clean
	$(MAKE) -C cmdb2xml clean
	$(MAKE) -C cmupd clean
	$(MAKE) -C cm2html clean
	$(MAKE) -C cmdoc clean
	$(MAKE) -C cmmdump clean
	$(MAKE) -C cmfileredirector clean
	$(MAKE) -C eh clean
	$(MAKE) -C rt clean
