# $Id: Makefile,v 1.2 2004/06/22 02:24:04 greear Exp $

BUILD = ../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libsoundcard

PACKAGES += RTP UTIL LIBXML LIBCEXT OPENSSL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ -L$(ROOT)/rtp/obj.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lgcc_s -lvutil -lrtp


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
