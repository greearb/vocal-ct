# $Id: Makefile,v 1.1 2004/05/01 04:15:23 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY=librtsp

PACKAGES += SDP PTHREAD UTIL LIBXML

# name of all files to be compiled (don't include header files)
SRC = \
     CharData.cxx \
     CharDataParser.cxx \
     RtspUtil.cxx \
     RtspMsg.cxx \
     RtspRequest.cxx \
     RtspResponse.cxx \
     RtspMsgParser.cxx \
     RtspSdp.cxx \
     RtspDescribeMsg.cxx \
     RtspAnnounceMsg.cxx \
     RtspPlayMsg.cxx \
     RtspRecordMsg.cxx \
     RtspSetupMsg.cxx \
     RtspPauseMsg.cxx \
     RtspTeardownMsg.cxx \
     RtspOptionsMsg.cxx \
     RtspSetParameterMsg.cxx \
     RtspTransportHdr.cxx \
     RtspRangeHdr.cxx \
     RtspLocationHdr.cxx \
     RtspRtpInfoHdr.cxx \
     RtspTcpBuffer.cxx \
     RtspTcpConnection.cxx \
     RtspTransceiver.cxx \

include $(BUILD)/Makefile.post
