
# $Log: Makefile,v $
# Revision 1.3  2004/11/19 01:54:38  greear
# First attempt at allowing a 32-bit compile on Opteron.  Had to fix
# some code that GCC 3.4 would not compile as well.
# --Ben
#
# Revision 1.2  2004/06/22 02:24:05  greear
# gua can now make calls to itself.  Have not tested registration yet.
# Have not driven through CLI yet...
# --Ben
#
# Revision 1.1.1.1  2004/05/01 04:15:38  greear
# Branching vocal
#
# Revision 1.3.16.2  2004/01/13 02:15:04  greearb
# Merge the bulk of my remaining changes.  Includes:
# *  Remove dependence on the deprecated thread code.  Will remove it from tree next.
# *  Make tcp-client socket code able to bind to local interface.
# *  Locking changes down in the SIP user-agent and related code.
#
# I tried to not back out any of the stund changes and a few other places
# where I noticed other folks had made changes since my last internal
# merge, but it's possible I missed something....
#
# --Ben Greear
#
# Revision 1.3  2002/05/29 01:16:27  bko
# fixes for repository building
#
# Revision 1.2  2001/01/02 22:29:14  deepalir
# *** empty log message ***
#
# Revision 1.1  2000/11/16 23:09:27  deepali
# Initial Creation
#

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += VMCP UTIL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ -L../vmcp/obj.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lvutil -lvmcp


#INCDIRS += codec

TARGET_LIBRARY = libvme

#TESTPROGRAMS = \
#        codec/encode.c \
#        codec/decode.c

#CODE_SUBDIRS = codec

# name of all files to be compiled (don't include header files)
SRC = \
	Event.cxx \
	State.cxx \
	Session.cxx \
	EventSource.cxx \
	LineVMCP.cxx \
	Timer.cxx \
	PromptServer.cxx \
	Waiter.cxx

#CAPIBUILDER := $(BUILD)/../tools/cApiBuilder
#
#rtpApi.h: rtpTools.hxx Rtp.hxx Rtcp.hxx RtpSession.hxx $(CAPIBUILDER)
#	cat rtpTools.hxx Rtp.hxx Rtcp.hxx RtpSession.hxx | $(CAPIBUILDER) -h > rtpApi.h
##
#rtpApi.cxx: rtpTools.hxx Rtp.hxx Rtcp.hxx RtpSession.hxx $(CAPIBUILDER)
#	cat rtpTools.hxx Rtp.hxx Rtcp.hxx RtpSession.hxx | $(CAPIBUILDER) -irtpTools.hxx -iRtp.hxx -iRtcp.hxx -iRtpSession.hxx   > rtpApi.cxx

include $(BUILD)/Makefile.post
