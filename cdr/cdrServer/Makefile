# $Id: Makefile,v 1.1 2004/05/01 04:14:55 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += CDRLIB PSLIB PSUTILIB HEARTBEAT UTIL LIBCEXT PTHREAD LIBXML OPENSSL

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
	CdrManager.cxx \
	CdrUserCache.cxx


include $(BUILD)/Makefile.post
