# $Id: Makefile,v 1.3 2004/11/19 01:54:38 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libsdp2

PACKAGES += UTIL  LIBXML OPENSSL LIBCEXT

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/
#MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lgcc_s -lvutil
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lvutil


ifeq ($(USE_YARROW),1)
PACKAGES += YARROW
endif

# name of all files to be compiled (don't include header files)
SRC = \
	Sdp2Session.cxx \
	Sdp2Media.cxx \
	Sdp2Attributes.cxx \
	Sdp2Time.cxx \
	Sdp2Bandwidth.cxx \
	Sdp2Connection.cxx \
	Sdp2Encryptkey.cxx \


include $(BUILD)/Makefile.post
