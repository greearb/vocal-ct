# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4-p6 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

# $Id: Makefile,v 1.1 2004/05/01 04:15:15 greear Exp $

# Disable automatic dependency tracking if using other tools than gcc and gmake
#AUTOMAKE_OPTIONS = no-dependencies


SHELL = /bin/sh

srcdir = .
top_srcdir = ..
prefix = /home/greear/btbits/btbits/l4libs/vocal/contrib/speex/speex-1.0.3/build
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/speex
pkglibdir = $(libdir)/speex
pkgincludedir = $(includedir)/speex

top_builddir = ..

ACLOCAL = /home/greear/btbits/btbits/l4libs/vocal/contrib/speex/speex-1.0.3/missing aclocal-1.4
AUTOCONF = autoconf
AUTOMAKE = /home/greear/btbits/btbits/l4libs/vocal/contrib/speex/speex-1.0.3/missing automake-1.4
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = i686-pc-linux-gnu
host_triplet = i686-pc-linux-gnu
AS = @AS@
CC = gcc
DLLTOOL = @DLLTOOL@
ECHO = echo
EXEEXT = 
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
MAINT = #
MAKEINFO = makeinfo
OBJDUMP = @OBJDUMP@
OBJEXT = o
OGG_INCLUDES = -I/usr/include
OGG_LDFLAGS = -L/usr/lib
OGG_LIBS = -logg
PACKAGE = speex
RANLIB = ranlib
SPEEX_LT_AGE = 1
SPEEX_LT_CURRENT = 2
SPEEX_LT_REVISION = 2
STRIP = strip
VERSION = 1.0.3
src = src

lib_LTLIBRARIES = libspeex.la

# Sources for compilation in the library
libspeex_la_SOURCES = nb_celp.c 	sb_celp.c 	lpc.c 	ltp.c 	lsp.c 	quant_lsp.c 	lsp_tables_nb.c 	gain_table.c 	gain_table_lbr.c 	cb_search.c 	filters.c 	bits.c 	modes.c 	vq.c 	high_lsp_tables.c 	vbr.c 	hexc_table.c 	exc_5_256_table.c 	exc_5_64_table.c 	exc_8_128_table.c 	exc_10_32_table.c 	exc_10_16_table.c 	exc_20_32_table.c 	hexc_10_32_table.c 	misc.c 	speex_header.c 	speex_callbacks.c 	math_approx.c 	stereo.c


include_HEADERS = speex.h 	speex_bits.h 	speex_header.h 	speex_callbacks.h 	speex_stereo.h


noinst_HEADERS = lsp.h 	nb_celp.h 	lpc.h 	ltp.h 	quant_lsp.h 	cb_search.h 	filters.h 	stack_alloc.h 	vq.h 	modes.h 	sb_celp.h 	vbr.h 	misc.h 	ltp_sse.h 	filters_sse.h 	math_approx.h


libspeex_la_LDFLAGS = -version-info 2:2:1

noinst_PROGRAMS = testenc testenc_wb testenc_uwb

testenc_SOURCES = testenc.c
testenc_LDADD = libspeex.la

testenc_wb_SOURCES = testenc_wb.c
testenc_wb_LDADD = libspeex.la

testenc_uwb_SOURCES = testenc_uwb.c
testenc_uwb_LDADD = libspeex.la
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(lib_LTLIBRARIES)


DEFS =  -DHAVE_DLFCN_H=1 -DHAVE_SYS_SOUNDCARD_H=1 -DHAVE_LIBOGG=1 -DHAVE_LIBM=1 -DVERSION=\"1.0.3\"  -I. -I$(srcdir) 
CPPFLAGS = 
LDFLAGS = 
LIBS = -lm 
libspeex_la_LIBADD = 
libspeex_la_OBJECTS =  nb_celp.lo sb_celp.lo lpc.lo ltp.lo lsp.lo \
quant_lsp.lo lsp_tables_nb.lo gain_table.lo gain_table_lbr.lo \
cb_search.lo filters.lo bits.lo modes.lo vq.lo high_lsp_tables.lo \
vbr.lo hexc_table.lo exc_5_256_table.lo exc_5_64_table.lo \
exc_8_128_table.lo exc_10_32_table.lo exc_10_16_table.lo \
exc_20_32_table.lo hexc_10_32_table.lo misc.lo speex_header.lo \
speex_callbacks.lo math_approx.lo stereo.lo
noinst_PROGRAMS =  testenc$(EXEEXT) testenc_wb$(EXEEXT) \
testenc_uwb$(EXEEXT)
PROGRAMS =  $(noinst_PROGRAMS)

testenc_OBJECTS =  testenc.$(OBJEXT)
testenc_DEPENDENCIES =  libspeex.la
testenc_LDFLAGS = 
testenc_wb_OBJECTS =  testenc_wb.$(OBJEXT)
testenc_wb_DEPENDENCIES =  libspeex.la
testenc_wb_LDFLAGS = 
testenc_uwb_OBJECTS =  testenc_uwb.$(OBJEXT)
testenc_uwb_DEPENDENCIES =  libspeex.la
testenc_uwb_LDFLAGS = 
CFLAGS = -g -O2
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
HEADERS =  $(include_HEADERS) $(noinst_HEADERS)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = gtar
GZIP_ENV = --best
SOURCES = $(libspeex_la_SOURCES) $(testenc_SOURCES) $(testenc_wb_SOURCES) $(testenc_uwb_SOURCES)
OBJECTS = $(libspeex_la_OBJECTS) $(testenc_OBJECTS) $(testenc_wb_OBJECTS) $(testenc_uwb_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .lo .o .obj .s
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps libspeex/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-libLTLIBRARIES:

clean-libLTLIBRARIES:
	-test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)

distclean-libLTLIBRARIES:

maintainer-clean-libLTLIBRARIES:

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "$(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  $(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
	done

.c.o:
	$(COMPILE) -c $<

# FIXME: We should only use cygpath when building on Windows,
# and only if it is available.
.c.obj:
	$(COMPILE) -c `cygpath -w $<`

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core
	-rm -f *.$(OBJEXT)

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

.c.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.s.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.S.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:

maintainer-clean-libtool:

libspeex.la: $(libspeex_la_OBJECTS) $(libspeex_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libspeex_la_LDFLAGS) $(libspeex_la_OBJECTS) $(libspeex_la_LIBADD) $(LIBS)

mostlyclean-noinstPROGRAMS:

clean-noinstPROGRAMS:
	-test -z "$(noinst_PROGRAMS)" || rm -f $(noinst_PROGRAMS)

distclean-noinstPROGRAMS:

maintainer-clean-noinstPROGRAMS:

testenc$(EXEEXT): $(testenc_OBJECTS) $(testenc_DEPENDENCIES)
	@rm -f testenc$(EXEEXT)
	$(LINK) $(testenc_LDFLAGS) $(testenc_OBJECTS) $(testenc_LDADD) $(LIBS)

testenc_wb$(EXEEXT): $(testenc_wb_OBJECTS) $(testenc_wb_DEPENDENCIES)
	@rm -f testenc_wb$(EXEEXT)
	$(LINK) $(testenc_wb_LDFLAGS) $(testenc_wb_OBJECTS) $(testenc_wb_LDADD) $(LIBS)

testenc_uwb$(EXEEXT): $(testenc_uwb_OBJECTS) $(testenc_uwb_DEPENDENCIES)
	@rm -f testenc_uwb$(EXEEXT)
	$(LINK) $(testenc_uwb_LDFLAGS) $(testenc_uwb_OBJECTS) $(testenc_uwb_LDADD) $(LIBS)

install-includeHEADERS: $(include_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(includedir)
	@list='$(include_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(includedir)/$$p"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(includedir)/$$p; \
	done

uninstall-includeHEADERS:
	@$(NORMAL_UNINSTALL)
	list='$(include_HEADERS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(includedir)/$$p; \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = libspeex

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done
bits.lo bits.o : bits.c speex_bits.h misc.h
cb_search.lo cb_search.o : cb_search.c cb_search.h speex_bits.h \
	filters.h stack_alloc.h vq.h misc.h
exc_10_16_table.lo exc_10_16_table.o : exc_10_16_table.c
exc_10_32_table.lo exc_10_32_table.o : exc_10_32_table.c
exc_20_32_table.lo exc_20_32_table.o : exc_20_32_table.c
exc_5_256_table.lo exc_5_256_table.o : exc_5_256_table.c
exc_5_64_table.lo exc_5_64_table.o : exc_5_64_table.c
exc_8_128_table.lo exc_8_128_table.o : exc_8_128_table.c
filters.lo filters.o : filters.c filters.h stack_alloc.h
gain_table.lo gain_table.o : gain_table.c
gain_table_lbr.lo gain_table_lbr.o : gain_table_lbr.c
hexc_10_32_table.lo hexc_10_32_table.o : hexc_10_32_table.c
hexc_table.lo hexc_table.o : hexc_table.c
high_lsp_tables.lo high_lsp_tables.o : high_lsp_tables.c
lpc.lo lpc.o : lpc.c lpc.h
lsp.lo lsp.o : lsp.c lsp.h stack_alloc.h
lsp_tables_nb.lo lsp_tables_nb.o : lsp_tables_nb.c
ltp.lo ltp.o : ltp.c ltp.h speex_bits.h stack_alloc.h filters.h
math_approx.lo math_approx.o : math_approx.c math_approx.h
misc.lo misc.o : misc.c misc.h
modes.lo modes.o : modes.c modes.h speex.h speex_bits.h ltp.h \
	quant_lsp.h cb_search.h sb_celp.h nb_celp.h speex_callbacks.h \
	vbr.h filters.h misc.h
nb_celp.lo nb_celp.o : nb_celp.c nb_celp.h modes.h speex.h speex_bits.h \
	speex_callbacks.h vbr.h filters.h lpc.h lsp.h ltp.h quant_lsp.h \
	cb_search.h stack_alloc.h vq.h misc.h
quant_lsp.lo quant_lsp.o : quant_lsp.c quant_lsp.h speex_bits.h
sb_celp.lo sb_celp.o : sb_celp.c sb_celp.h modes.h speex.h speex_bits.h \
	nb_celp.h speex_callbacks.h vbr.h filters.h lpc.h lsp.h \
	stack_alloc.h cb_search.h quant_lsp.h vq.h ltp.h misc.h
speex_callbacks.lo speex_callbacks.o : speex_callbacks.c \
	speex_callbacks.h speex.h speex_bits.h misc.h
speex_header.lo speex_header.o : speex_header.c speex_header.h misc.h \
	speex.h speex_bits.h
stereo.lo stereo.o : stereo.c speex_stereo.h speex_bits.h \
	speex_callbacks.h speex.h vq.h
testenc.o: testenc.c speex.h speex_bits.h speex_callbacks.h
testenc_uwb.o: testenc_uwb.c speex.h speex_bits.h
testenc_wb.o: testenc_wb.c speex.h speex_bits.h
vbr.lo vbr.o : vbr.c vbr.h
vq.lo vq.o : vq.c vq.h

info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am: install-libLTLIBRARIES
install-exec: install-exec-am

install-data-am: install-includeHEADERS
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-libLTLIBRARIES uninstall-includeHEADERS
uninstall: uninstall-am
all-am: Makefile $(LTLIBRARIES) $(PROGRAMS) $(HEADERS)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(libdir) $(DESTDIR)$(includedir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-libLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-noinstPROGRAMS \
		mostlyclean-tags mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-libLTLIBRARIES clean-compile clean-libtool \
		clean-noinstPROGRAMS clean-tags clean-generic \
		mostlyclean-am

clean: clean-am

distclean-am:  distclean-libLTLIBRARIES distclean-compile \
		distclean-libtool distclean-noinstPROGRAMS \
		distclean-tags distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-libLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-noinstPROGRAMS maintainer-clean-tags \
		maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-libLTLIBRARIES distclean-libLTLIBRARIES \
clean-libLTLIBRARIES maintainer-clean-libLTLIBRARIES \
uninstall-libLTLIBRARIES install-libLTLIBRARIES mostlyclean-compile \
distclean-compile clean-compile maintainer-clean-compile \
mostlyclean-libtool distclean-libtool clean-libtool \
maintainer-clean-libtool mostlyclean-noinstPROGRAMS \
distclean-noinstPROGRAMS clean-noinstPROGRAMS \
maintainer-clean-noinstPROGRAMS uninstall-includeHEADERS \
install-includeHEADERS tags mostlyclean-tags distclean-tags clean-tags \
maintainer-clean-tags distdir info-am info dvi-am dvi check check-am \
installcheck-am installcheck install-exec-am install-exec \
install-data-am install-data install-am install uninstall-am uninstall \
all-redirect all-am all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
