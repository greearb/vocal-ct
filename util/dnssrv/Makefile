# $Id: Makefile,v 1.1 2004/05/01 04:15:33 greear Exp $

# $Log: Makefile,v $
# Revision 1.1  2004/05/01 04:15:33  greear
# Initial revision
#
# Revision 1.3  2001/05/15 15:51:41  wbreu
# remove commented line
#
# Revision 1.2  2001/04/20 21:00:34  wbreu
# since util/Makefile is picking up Dns* files, update this
# Makefile to only build testDnsResolver executable.
#
# Revision 1.1  2001/03/27 23:51:44  wbreu
# Check in DnsResolver utility - initial version
#
# Revision 1.1  1999/11/03 23:01:16  bogawa
#
# add make file for the util directory
#

BUILD = ../../build

include $(BUILD)/Makefile.pre

DEFINES += 

TARGET_LIBRARY = 


PACKAGES +=  UTIL BIND  PTHREAD

# names of programs (e.g. test programs) to be compiled
TESTPROGRAMS = \
		testDnsResolver.cxx


# name of all files to be compiled (don't include header files)



SRC =

include $(BUILD)/Makefile.post
