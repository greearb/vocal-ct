# $Id: Makefile,v 1.1 2004/05/01 04:15:23 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

DEFINES += USE_PTHREADS

PACKAGES += PTHREAD RTP UTIL QUICKNET
#PACKAGES += PTHREAD RTP UTIL

#INCDIRS += . ../util/


TESTPROGRAMS = \
	soundcardTest.cxx \
    rtpPlayLoad.cxx \
    rtpPlay.cxx \
    rtpPlayRaw.cxx \
    rtpRecord.cxx \
    test_endian.cxx

#    test_allocate_two_rtpreceivers.cxx \
#    maxUdpRecording.cxx \
#    maxUdpPlaying.cxx \
#    maxPlaying.cxx \
#    maxRecording.cxx \
#    maxRecordingNF.cxx \
#    maxRtpSessions.cxx \
#    sampleUsage.cxx \
#    rtpRecord.cxx \
#    phoneJack.cxx \
#    rtpSimple.cxx

# name of all files to be compiled (don't include header files)
SRC = \


include $(BUILD)/Makefile.post
