# $Id: Makefile,v 1.1 2004/05/01 04:15:16 greear Exp $

BUILD = ../build

CXXFLAGS += -I. 
LDLIBS += -lm 

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libmedia 

PACKAGES += RTP MEDIALIB SDP2 PTHREAD UTIL OPENSSL


MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ -L$(ROOT)/rtp/obj.$(TARGET_NAME)/ \
  -L$(ROOT)/sdp/sdp2/obj.$(TARGET_NAME)/ \
  -L$(ROOT)/contrib/speex.$(TARGET_NAME)/build/lib \
  -L$(ROOT)/contrib/Equivalence.$(OS_ARCH)/openh323-1.11.4/lib

MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lpthread -lstdc++ -lm -lgcc_s -lvutil -lrtp -lm -lsdp2 -lspeex


INCDIRS += $(ROOT)/contrib/speex.$(TARGET_NAME)/build/include
LIBDIRS += $(ROOT)/contrib/speex.$(TARGET_NAME)/build/lib
LDLIBS_LAST += -lspeex

ifeq ($(VOCAL_USE_LANFORGE),yes)
LFDIR=../../..
INCDIRS += $(LFDIR)/include
LIBDIRS += $(LFDIR)/lib
LDLIBS_LAST += -lgcont -lstring2
endif

ifeq ($(VOCAL_USE_VOICEAGE),yes)
MKSHLIB_INCDIRS += -L../../VoiceAge
MKSHLIB_LIBS += -lCodecLib -lg729 -lg729utils
LIBDIRS += ../../VoiceAge
LDLIBS_LAST += -lCodecLib -lg729 -lg729utils
endif


TESTPROGRAMS = 

SRC = \
AudioThread.cxx \
MediaDevice.cxx \
CodecG711U.cxx \
CodecAdaptor.cxx \
CodecSpeex.cxx \
CodecG726_16.cxx \
CodecG726_24.cxx \
CodecG726_32.cxx \
CodecG726_40.cxx \
CodecTelEvent.cxx \
MediaCapability.cxx \
MediaController.cxx \
MediaSession.cxx \
NetworkRes.cxx \
SdpHandler.cxx \
MRtpSession.cxx

ifeq ($(VOCAL_USE_VOICEAGE),yes)
SRC += CodecG729a.cxx
endif

#docs:	doc++/config.txt
#	doc++ --config doc++/config.txt

include $(BUILD)/Makefile.post

