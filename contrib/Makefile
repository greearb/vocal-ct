# $Id: Makefile,v 1.8 2006/03/14 00:20:07 greear Exp $

BUILD = ../build
include $(BUILD)/Makefile.pre
include $(BUILD)/Makefile.pkg

ifeq ($(OSTYPE),SunOS)
ifeq ($(VOCAL_TOOLCHAIN_TYPE),gnu)
IMAP_OS_ARG = gso
endif
ifeq ($(VOCAL_TOOLCHAIN_TYPE),sunpro)
IMAP_OS_ARG = fso
endif
endif

ifeq ($(OSTYPE),FreeBSD)
IMAP_OS_ARG = bsf
endif

ifeq ($(OSTYPE),OpenBSD)
IMAP_OS_ARG = bso
endif

ifeq ($(OSTYPE),Linux)
IMAP_OS_ARG = lnp
endif


MAKE_NOJ = $(patsubst %-j4,%, $(patsubst %-j2,%, $(MAKE)))

INSTALL = install

XML_VER=2.6.20

#CODE_DEPENDS = libxml
CODE_DEPENDS =
ifeq ($(VOCAL_USE_SPEEX),yes)
CODE_DEPENDS = speex_targ
endif
#CODE_DEPENDS += Equivalence

ifeq ($(OSTYPE),FreeBSD)
CODE_DEPENDS += libcext_bsd libcext_lgpl
endif

#ifeq ($(OSTYPE),Darwin)
#CODE_DEPENDS += libcext_bsd libcext_lgpl
#endif

ifeq ($(OSTYPE),SunOS)
CODE_DEPENDS += libcext_bsd
endif

ifeq ($(USE_YARROW),1)
CODE_DEPENDS += yarrow
endif

ifeq ($(VOCAL_HAS_ENUM),1)
CODE_DEPENDS += $(wildcard enum)
endif

#CODE_DEPENDS += $(wildcard quicknet andreasen hitachi)

code: $(CODE_DEPENDS)

quicknet:
	cd quicknet ; $(MAKE)

andreasen:
	mkdir -p andreasen
	cd andreasen ; $(MAKE)

hitachi:
ifeq ($(OS_ARCH),sh)
	cd hitachi ; $(MAKE)
endif

.PHONY: libxml Equivalence libcext imap osp xerces-c-src1_4_0 andreasen quicknet hitachi

ifeq ($(VOCAL_USE_SYSTEM_IMAP),true)
imap:
	@echo "using system IMAP"
else
imap: imap.$(OS_ARCH)
	$(LNDIR) ../imap imap.$(OS_ARCH)
	(cd imap.$(OS_ARCH) && MAKEFLAGS= MAKELEVEL= CC="$(CC)" $(MAKE) $(IMAP_OS_ARG))

imap.$(OS_ARCH):
	-mkdir imap.$(OS_ARCH) 
endif

yarrow: yarrow.$(OS_ARCH)
	$(LNDIR) ../yarrow yarrow.$(OS_ARCH)
	cd yarrow.$(OS_ARCH) ; $(MAKE_NOJ) 

yarrow.$(OS_ARCH):
	-mkdir yarrow.$(OS_ARCH)

enum: enum.$(OS_ARCH)
	cd enum.$(OS_ARCH) ; $(MAKE)

enum.$(OS_ARCH):
	cp -r enum enum.$(OS_ARCH)
	-mkdir -p enum.$(OS_ARCH)/bin enum.$(OS_ARCH)/lib
	cd enum.$(OS_ARCH) ; cp Makefile.$(OS_ARCH) makefile ; $(MAKE)


ifeq ($(VOCAL_USE_SYSTEM_XERCES_C),true)

xerces-c: 
	@echo "using system IMAP"
else
xerces-c: xerces-c-src1_4_0

xerces-c-src1_4_0: xerces-c-src1_4_0.$(OS_ARCH) $(XERCES_DEPENDS)

$(XERCES_DEPENDS):
	XERCESCROOT="`pwd`/xerces-c-src1_4_0.$(OS_ARCH)"; cd xerces-c-src1_4_0.$(OS_ARCH)/src; \
	export XERCESCROOT; $(MAKE_NOJ)

ifeq ($(OSTYPE),SunOS)
XERCES_CONFIGURE=-psolaris -ccc -xCC -minmem -nfileonly -tnative
else
  ifeq ($(OSTYPE),Linux)
    XERCES_CONFIGURE=-plinux -c"$(CC)" -x"$(CXX)" -minmem -nfileonly -tnative
  endif
endif

xerces-c-src1_4_0.$(OS_ARCH):
	$(LNDIR) ../xerces-c-src1_4_0 xerces-c-src1_4_0.$(OS_ARCH)
	XERCESCROOT="`pwd`/xerces-c-src1_4_0.$(OS_ARCH)"; cd xerces-c-src1_4_0.$(OS_ARCH)/src; export XERCESCROOT; chmod u+x runConfigure; chmod u+x configure; ./runConfigure $(XERCES_CONFIGURE)

endif


ifeq ($(VOCAL_USE_SYSTEM_LIBXML),true)

libxml:
	@echo "using system libxml2"
else
libxml: ../lib.$(TARGET_NAME)/libvxml.so

../lib.$(TARGET_NAME)/libvxml.so: libxmlB
	mkdir -p ../lib.$(TARGET_NAME)
	rm -f $@
	rm -f ../lib.$(TARGET_NAME)/libvxml.a
	ln -s -f $(shell pwd)/libxml2.$(TARGET_NAME)/.libs/libxml2.so.${XML_VER} $@
	ln -s -f $(shell pwd)/libxml2.$(TARGET_NAME)/.libs/libxml2.a ../lib.$(TARGET_NAME)/libvxml.a

libxmlB: libxml2.$(TARGET_NAME)
	(cd libxml2.$(TARGET_NAME) ; CC="$(CC)" $(MAKE))
	(cd libxml2.$(TARGET_NAME)/.libs ; ln -s -f libxml2.so libvxml.so; ln -s -f libxml2.a libvxml.a)

libxml2.$(TARGET_NAME):
	tar -xvzf libxml2-${XML_VER}.tar.gz
	$(LNDIR) ../libxml2-${XML_VER} libxml2.$(TARGET_NAME)
	(cd libxml2.$(TARGET_NAME) ; CC="$(CC)" $(CONFIGURE) --without-threads --without-python)
endif

libcext: libcext_bsd libcext_lgpl


libcext_bsd: libcext_bsd.$(OS_ARCH)
	$(LNDIR) ../libcext_bsd libcext_bsd.$(OS_ARCH)
	cd libcext_bsd.$(OS_ARCH) ; $(MAKE_NOJ)

libcext_bsd.$(OS_ARCH):
	-mkdir libcext_bsd.$(OS_ARCH)


libcext_lgpl: libcext_lgpl.$(OS_ARCH)
	$(LNDIR) ../libcext_lgpl libcext_lgpl.$(OS_ARCH)
	cd libcext_lgpl.$(OS_ARCH) ; $(MAKE_NOJ)

libcext_lgpl.$(OS_ARCH):
	-mkdir libcext_lgpl.$(OS_ARCH)


# Speex support
ifeq ($(VOCAL_USE_SPEEX),yes)
speex_targ: ../lib.$(TARGET_NAME)/libspeex.so.1
else
speex_targ:
	echo "Not building speex."
endif

speex_targB: speex.$(TARGET_NAME);
	(cd speex.$(TARGET_NAME); CC="$(CC)" $(MAKE); CC="$(CC)" $(MAKE) install)

../lib.$(TARGET_NAME)/libspeex.so.1:  speex_targB
	mkdir -p ../lib.$(TARGET_NAME)
	rm -f $@
	ln -s -f $(shell pwd)/speex.$(TARGET_NAME)/build/lib/libsndfile.so.1 $@

speex.$(TARGET_NAME):
	mkdir -p ../speex/speex-$(SPEEX_VER)/build
	$(LNDIR) ../speex/speex-$(SPEEX_VER) speex.$(TARGET_NAME)
	(cd speex.$(TARGET_NAME) ; CC="$(CC)" $(CONFIGURE) --prefix=`pwd`/build)



ifeq ($(VOCAL_USE_SYSTEM_LIBSNDFILE),yes)
newsndfile:
	@echo "using system newsndfile"
else
newsndfile: ../lib.$(TARGET_NAME)/libsndfile.so.1

newsndfileB: newsndfile.$(OS_ARCH)
	(cd newsndfile.$(OS_ARCH) ; CC="$(CC)" $(MAKE_NOJ))

../lib.$(TARGET_NAME)/libsndfile.so.1:  newsndfileB
	mkdir -p ../lib.$(TARGET_NAME)
	ln -s -f $(shell pwd)/newsndfile.$(OS_ARCH)/src/.libs/libsndfile.so.1 $@



ifeq ($(OSTYPE),SunOS)
newsndfile.$(OS_ARCH):
	$(LNDIR) ../$(SNDFILEVERSION) newsndfile.$(OS_ARCH)
	ln -s /usr/include/assert.h newsndfile.$(OS_ARCH)/src/GSM610/assert.h
	(cd newsndfile.$(OS_ARCH) ; CC="$(CC)" $(CONFIGURE))
else
newsndfile.$(OS_ARCH):
	$(LNDIR) ../$(SNDFILEVERSION) newsndfile.$(OS_ARCH)
ifeq ($(VOCAL_TOOLCHAIN_TYPE),armcross)
	(cd newsndfile.$(OS_ARCH) ; CC="$(CC)" $(CONFIGURE) --disable-gcc-opt)
	echo '#define GUESS_LITTLE_ENDIAN 1' >> newsndfile.$(OS_ARCH)/src/config.h
else
	(cd newsndfile.$(OS_ARCH) ; CC="$(CC)" $(CONFIGURE))


#	(cd newsndfile.$(OS_ARCH) ; ./ltconfig ltmain.sh; aclocal; CC="$(CC)" $(CONFIGURE))

endif
endif
endif


ifeq ($(OSTYPE), SunOS)
OSP_COMPILER = -o use_spro
endif

osp: osp.$(OS_ARCH)/dependencies
	cd osp.$(OS_ARCH)/src ; OSTYPE=$(OSTYPE) ; export OSTYPE ; OS_ARCH=$(OS_ARCH) ; export OS_ARCH ; ./osp_sdk_compile.ksh -d client $(OSP_COMPILER)

osp.$(OS_ARCH)/dependencies: osp.$(OS_ARCH)
	$(LNDIR) ../OSPToolkit-2.5.2 osp.$(OS_ARCH)
	touch osp.$(OS_ARCH)/dependencies

osp.$(OS_ARCH):
	-mkdir osp.$(OS_ARCH)


Equivalence: Equivalence.$(OS_ARCH) 
	cd Equivalence.$(OS_ARCH) && $(MAKE_NOJ)

Equivalence.$(OS_ARCH):
	mkdir Equivalence.$(OS_ARCH)
	cd Equivalence.$(OS_ARCH) && $(LNDIR) ../Equivalence

install: all
	cd quicknet ; $(MAKE) install

doc:

cleanall: clean

clean:
	-rm -rf xerces-c-src1_4_0.$(OS_ARCH)
	-rm -rf libxml2.$(TARGET_NAME)
	-rm -rf libxml2-${XML_VER}
	-rm -rf Equivalence.$(OS_ARCH)
	-rm -rf yarrow.$(OS_ARCH)
	-rm -rf newsndfile.$(OS_ARCH)
	-rm -rf speex.*
	-rm -f speex/speex-$(SPEEX_VER)/config.cache
	-rm -rf imap.$(OS_ARCH)
	-if [ -d yarrow ]; then cd yarrow ; $(MAKE) clean; fi
	-if [ -d quicknet ]; then cd quicknet; $(MAKE) clean; fi
	-if [ -d speex/speex-$(SPEEX_VER) ]; then cd speex/speex-$(SPEEX_VER); $(MAKE) clean; fi
	-rm -fr speex/speex-$(SPEEX_VER)/build
	-if [ -d andreasen ]; then cd andreasen ; $(MAKE) clean; fi


