# $Id: Makefile,v 1.3 2004/11/19 01:54:37 greear Exp $

BUILD = ../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libsoundcard

PACKAGES += RTP UTIL LIBCEXT OPENSSL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ -L$(ROOT)/rtp/obj.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lvutil -lrtp


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
