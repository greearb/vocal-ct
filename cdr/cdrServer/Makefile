# $Id: Makefile,v 1.3 2004/11/19 01:54:37 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += CDRLIB PSLIB SIP PROXYBASE PSUTILIB HEARTBEAT UTIL LIBCEXT PTHREAD LIBXML OPENSSL SDP2

# names of programs (e.g. test programs) to be compiled
TESTPROGRAMS = cdrserv.cxx

# name of all files to be compiled (don't include header files)
SRC = \
	CdrConfig.cxx \
	CdrCache.cxx \
	CdrFileHandler.cxx \
	CdrMarshal.cxx \
	CdrServer.cxx \
	CdrBilling.cxx \
	RadiusStack.cxx \
	MindClient.cxx \
	EventObj.cxx \
	CdrManager.cxx


include $(BUILD)/Makefile.post
