# $Id: Makefile,v 1.3 2004/11/19 01:54:38 greear Exp $

BUILD = ../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libsdp

PACKAGES += UTIL LIBXML

#MKSHLIB_LIBS += -lstdc++ -lm -lgcc_s
MKSHLIB_LIBS += -lstdc++ -lm


# names of programs (e.g. test programs) to be compiled
#TESTPROGRAMS = \
#	SdpTest.cxx

# name of all files to be compiled (don't include header files)
#  These all seem deprecated!!!  Wasted a day figuring that out :(
#SRC = \
#	SdpSession.cxx \
#	SdpMedia.cxx \
#	SdpAttributes.cxx \
#	SdpTime.cxx \
#	SdpBandwidth.cxx \
#	SdpConnection.cxx \
#	SdpEncryptkey.cxx


include $(BUILD)/Makefile.post
