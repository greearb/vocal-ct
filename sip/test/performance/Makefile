# $Id: Makefile,v 1.1 2004/05/01 04:15:26 greear Exp $

BUILD = ../../../build

include $(BUILD)/Makefile.pre

PACKAGES += PTHREAD SIP SDP2 SIPMGCP UTIL LIBCEXT LIBXML

CXXFLAGS += -DPERF_DEBUG	

TESTPROGRAMS += \
        perfsendInviteAck.cxx \
	perfrecvInvite.cxx 

NOCARE= \
        perfParseHeaders.cxx \
        perfsendIdentInvite.cxx \
        perfrecvIdentInvite.cxx \
        perfParseInvite.cxx \
	perfsendInvite.cxx \
        perfsendInviteAck.cxx \
        perfParseInvite.cxx \
	TimeTransactionId.cxx \
	TimeSipDate.cxx \
	TimeSipUrl.cxx \
	TimeInviteAckPath.cxx



include $(BUILD)/Makefile.post
