
BUILD = ../build

include $(BUILD)/Makefile.pre

PACKAGES += UTIL LIBCEXT OPENSSL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ \
		   -L$(ROOT)/lib.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lvutil

ifeq ($(ISMINGW),1)
MKSHLIB_LIBS += -lws2_32 -lwinmm -lvutil
LDLIBS_LAST += -lws2_32 -lwinmm -lvutil
endif

INCDIRS += codec

TARGET_LIBRARY = librtp

#TESTPROGRAMS = \
#        codec/encode.c \
#        codec/decode.c

CODE_SUBDIRS = codec

# name of all files to be compiled (don't include header files)
SRC = \
	RtpPacket.cxx \
	RtpTransmitter.cxx \
	RtpReceiver.cxx \
	RtcpPacket.cxx \
	RtcpTransmitter.cxx \
	RtcpReceiver.cxx \
	RtpEvent.cxx \
	RtpEventReceiver.cxx \
	RtpSession.cxx \
	NtpTime.cxx \
	rtpTools.cxx \
        rtpCodec.cxx \
        codec/g711.c \
        codec/g721.c \
        codec/g723_24.c \
        codec/g723_40.c \
        codec/g72x.c \
	codec/g726_16.c \
	codec/g726_24.c \
	codec/g726_32.c \
	codec/g726_40.c \
	codec/g726.c

CAPIBUILDER := $(BUILD)/../tools/cApiBuilder

rtpApi.h: rtpTools.hxx Rtp.hxx Rtcp.hxx RtpSession.hxx $(CAPIBUILDER)
	cat rtpTools.hxx Rtp.hxx Rtcp.hxx RtpSession.hxx | $(CAPIBUILDER) -h > rtpApi.h

rtpApi.cxx: rtpTools.hxx Rtp.hxx Rtcp.hxx RtpSession.hxx $(CAPIBUILDER)
	cat rtpTools.hxx Rtp.hxx Rtcp.hxx RtpSession.hxx | $(CAPIBUILDER) -irtpTools.hxx -iRtp.hxx -iRtcp.hxx -iRtpSession.hxx   > rtpApi.cxx

ifeq ($(VOCAL_USE_LANFORGE),yes)
LFDIR=$(l4LIBS_BASE)/..
#LFDIR=$(HOME)/btbits/btbits/
INCDIRS += $(LFDIR)/include
LIBDIRS += $(LFDIR)/lib
MKSHLIB_INCDIRS += -L$(LFDIR)/lib
MKSHLIB_LIBS += -lgcont
LDLIBS_LAST += -lgcont
endif

include $(BUILD)/Makefile.post
