# $Id: Makefile,v 1.1 2004/05/01 04:15:16 greear Exp $

BUILD = ../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libsoundcard

PACKAGES += RTP UTIL PTHREAD LIBXML LIBCEXT OPENSSL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ -L$(ROOT)/rtp/obj.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lpthread -lstdc++ -lm -lgcc_s -lvutil -lrtp


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
