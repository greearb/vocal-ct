
# $Log: Makefile,v $
# Revision 1.1  2004/05/01 04:15:23  greear
# Initial revision
#
# Revision 1.2.86.2  2004/01/13 02:14:33  greearb
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
# Revision 1.2  2000/06/21 01:03:57  kle
# Fixed cycle counting.  Added RtpTransmitter::transmitRaw(char* data, int len).
# Reduced memory copies when receiving and transmitting packets.  No changes in RTCP.
#
# Revision 1.1  2000/01/06 02:37:18  wenqing
# codec conversion
#

BUILD = ../../build

include $(BUILD)/Makefile.pre

DEFINES += USE_PTHREADS

PACKAGES += 


#INCDIRS += . ../util/


TESTPROGRAMS = \
#        encode.c \
#        decode.c


# name of all files to be compiled (don't include header files)
SRC = \
        g711.c g72x.c g721.c g723_24.c g723_40.c g726_40.c g726_32.c g726_24.c g726_16.c

include $(BUILD)/Makefile.post
