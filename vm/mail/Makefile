# $Id: Makefile,v 1.2 2004/06/22 02:24:04 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += LIBIMAP UTIL PAM

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ -L$(ROOT)/contrib/imap.$(OS_ARCH)/c-client
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lgcc_s -lvutil -lpam -lnsl -limap

ifeq ($(USE_SSL),1)
PACKAGES += OPENSSL
endif

TARGET_LIBRARY = libmail

# name of all files to be compiled (don't include header files)
SRC = \
	mailx.cxx

include $(BUILD)/Makefile.post
