# $Id: Makefile,v 1.1 2004/05/01 04:15:25 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += PTHREAD RTP

TARGET_LIBRARY = libdrivers

TESTPROGRAMS = AudioTest.cxx

# here is the thing to enable the SIP filter

SRC = \
	OSSAudio.cxx

include $(BUILD)/Makefile.post
