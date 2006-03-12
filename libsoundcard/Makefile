# $Id: Makefile,v 1.4 2006/03/12 07:41:28 greear Exp $

BUILD = ../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libsoundcard

PACKAGES += RTP UTIL LIBCEXT OPENSSL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ -L$(ROOT)/rtp/obj.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lvutil -lrtp

ifeq ($(ARCH),i386-mingw32msvc)
MKSHLIB_LIBS += -lws2_32 -lwinmm -lvutil
LDLIBS_LAST += -lws2_32 -lwinmm -lvutil
endif


ifeq ($(USE_SSL),1)
PACKAGES += OPENSSL
endif

ifeq ($(OSTYPE),FreeBSD)
    INCDIRS += /usr/local/include
endif

TESTPROGRAMS = \
        testsound.cxx

SRC = \
	SoundCard.cxx

include $(BUILD)/Makefile.post
