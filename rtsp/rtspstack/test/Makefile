# $Id: Makefile,v 1.1 2004/05/01 04:15:23 greear Exp $

# $Log: Makefile,v $
# Revision 1.1  2004/05/01 04:15:23  greear
# Initial revision
#
# Revision 1.7  2001/02/17 02:51:18  wjin
# fix bug
#
# Revision 1.6  2001/02/17 00:02:28  kimle
# Added new states and processing
#
# Revision 1.5  2001/02/16 04:08:19  kimle
# more chnages
#
# Revision 1.4  2001/02/15 02:38:46  wjin
# add test file for encoder
#
# Revision 1.3  2001/02/13 02:39:53  kimle
# Fixed transceiver to handle multiple connections.
# Modified RtspRangeHdr.hxx RtspTransportHdr.hxx to compile
# Modified test programs to get transceiver changes
#
# Revision 1.2  2001/02/09 02:33:46  kimle
# transciever changes for multiple connections
#
# Revision 1.1  2001/02/08 22:45:15  wjin
# Initial check ins for RTSP
#

BUILD = ../../../build

include $(BUILD)/Makefile.pre

#TARGET_LIBRARY=librtsp.a

PACKAGES += RTSP SDP PTHREAD UTIL

TESTPROGRAMS = \
    RtspSdpTest.cxx \
    RtspParserTest.cxx \
    RtspDecoderTest.cxx \
    RtspEncoderTest.cxx \
    RtspTcpServerTest.cxx \
    RtspTransceiverTest.cxx \

# name of all files to be compiled (don't include header files)
SRC = \

include $(BUILD)/Makefile.post
