## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@ \
  -I pixmaps \
  -I ../../../../build

bin_PROGRAMS = sipset

sipset_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
  configuration.c configuration.h \
	registered.xpm unregistered.xpm

sipset_LDADD = @GTK_LIBS@ $(INTLLIBS)

