# $Id: Makefile,v 1.1 2004/05/01 04:15:24 greear Exp $

BUILD = ../../../build

include $(BUILD)/Makefile.pre


PACKAGES += SDP2 UTIL PTHREAD
ifeq ($(USE_SSL),1)
PACKAGES += OPENSSL
endif
ifeq ($(USE_YARROW),1)
PACKAGES += YARROW
endif
ifeq ($(USE_SSL),1)
PACKAGES += OPENSSL
endif

# name of all files to be compiled (don't include header files)
TESTPROGRAMS = \
	Sdp2Example.cxx 

include $(BUILD)/Makefile.post
