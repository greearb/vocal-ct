# $Id: Makefile,v 1.1 2004/05/01 04:15:38 greear Exp $

BUILD = ../../../build

include $(BUILD)/Makefile.pre

DEFINES += USE_PTHREADS

PACKAGES += PTHREAD VME VMCP UTIL
#PACKAGES += PTHREAD RTP UTIL

#INCDIRS += . ../util/


TESTPROGRAMS = \
	vmePlay.cxx \
	vmeRecord.cxx \
	vmeSession.cxx \
	vmePlayPrompt.cxx \
	vmePlayFrom.cxx \
	vmePlayTime.cxx

# name of all files to be compiled (don't include header files)
#SRC = \
#	SessionData.cxx


include $(BUILD)/Makefile.post
