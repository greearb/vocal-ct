# $Id: Makefile,v 1.1 2004/05/01 04:14:55 greear Exp $

# $Log: Makefile,v $
# Revision 1.1  2004/05/01 04:14:55  greear
# Initial revision
#
# Revision 1.6  2000/07/12 20:44:38  fhong
# Added callGen
#
# Revision 1.5  2000/06/01 03:52:15  fhong
# Added PTHREADS lib
#
# Revision 1.4  2000/05/22 22:43:45  fhong
# Removed the PSUTIL package, no longer necessary
#
# Revision 1.3  2000/04/12 00:27:05  fhong
# Changed the order of the libs, UTIL should be last
#
# Revision 1.2  2000/04/11 01:08:34  fhong
# Added PSUTILLIB to get tcp stuff
#
# Revision 1.1  2000/03/03 01:27:26  fhong
# These are test programs for the CDR API
#
# Revision 1.1  2000/01/20 03:09:12  fhong
# Create cdrServer
#

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += CDRLIB UTIL PTHREAD

# names of programs (e.g. test programs) to be compiled
TESTPROGRAMS = cdrclient.cxx callGen.cxx

# name of all files to be compiled (don't include header files)
SRC = 


include $(BUILD)/Makefile.post
