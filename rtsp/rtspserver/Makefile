# $Id: Makefile,v 1.1 2004/05/01 04:15:23 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

# Loops to a beginning of a file after reading to end of file
#DEFINES += INFINITE_READ

# Pretends to write file, but doesn't really.  Used for testing.
#DEFINES += DUMB_WRITE

# Uses *.sdp file
#DEFINES += SDP_FILE

# RealAudio hack - misinterperates a=control
#DEFINES += REAL_AUDIO

# Uses non random sessionIds.  Used for testing.
DEFINES += NONRANDOM_SESSIONID

# Surpress sending 200 for teardown request
#DEFINES += NO_200_FOR_TEARDOWN

# Optimize build
#DEFINES += CODE_OPTIMIZE

PACKAGES += LIBSNDFILE RTSPIF RTSP RTP SDP PTHREAD UTIL

TESTPROGRAMS = \
    mediaServer.cxx \

# name of all files to be compiled (don't include header files)
SRC = \
    OpAnnounce.cxx \
    OpDescribe.cxx \
    OpSetup.cxx \
    OpSetupInvalid.cxx \
    OpPlay.cxx \
    OpPlayInvalid.cxx \
    OpRecord.cxx \
    OpRecordInvalid.cxx \
    OpPause.cxx \
    OpPauseOk.cxx \
    OpPauseInvalid.cxx \
    OpTeardown.cxx \
    OpTeardownOk.cxx \
    OpOptions.cxx \
    OpSetParameter.cxx \
    StateMachine.cxx \
    State.cxx \
    RtpFileTypes.cxx \
    RtspCommandLine.cxx \
    RtspConfiguration.cxx \
    RtspFileHandler.cxx \
    RtspFileManager.cxx \
    RtspFile.cxx \
    RtspFileAu.cxx \
    RtspFileRaw.cxx \
    RtspFileRtp.cxx \
    RtspFileWav.cxx \
    RtspRtpProcessor.cxx \
    RtspSession.cxx \
    RtspSessionManager.cxx \
    RtspServer.cxx \
    RtspServerStats.cxx \
    RtspSessionStats.cxx \

include $(BUILD)/Makefile.post
