# $Id: Makefile,v 1.3 2004/11/19 01:54:37 greear Exp $

# $Log: Makefile,v $
# Revision 1.3  2004/11/19 01:54:37  greear
# First attempt at allowing a 32-bit compile on Opteron.  Had to fix
# some code that GCC 3.4 would not compile as well.
# --Ben
#
# Revision 1.2  2004/06/22 02:24:04  greear
# gua can now make calls to itself.  Have not tested registration yet.
# Have not driven through CLI yet...
# --Ben
#
# Revision 1.1.1.1  2004/05/01 04:14:55  greear
# Branching vocal
#
# Revision 1.8.22.2  2004/01/13 02:14:01  greearb
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
# Revision 1.8  2002/05/02 02:51:09  jason
# added template repositories -frepo
# added support for gcc3 toolchain
# added support for shared libraries
#
# Revision 1.7  2000/07/05 22:57:12  hyu
# Merging BRANCH_CURD into HEAD.
#
# Revision 1.6.8.1  2000/06/27 05:05:16  fhong
# Removed CdrConfig.cxx, moved to cdrServer/
#
# Revision 1.6  2000/05/22 22:43:16  fhong
# Removed the PSUTIL package, no longer necessary
#
# Revision 1.5  2000/05/17 01:26:10  fhong
# Added CdrData.cxx
#
# Revision 1.4  2000/04/18 04:55:35  fhong
# Added new file CdrConfig.cxx
#
# Revision 1.3  2000/04/12 00:26:13  fhong
# changed the order of the libs, UTIL should be last
#
# Revision 1.2  2000/04/10 04:26:26  fhong
# Added PSUTILLIB
#
# Revision 1.1  2000/03/03 01:21:58  fhong
# These are the new files for the CDR api
#
# Revision 1.1  2000/02/22 23:23:40  fhong
#
# Objects to get CDR information .
#

BUILD = ../../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libcdr

PACKAGES += UTIL LIBXML

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ -L$(ROOT)/lib.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lvxml -lvutil


# name of all files to be compiled (don't include header files)
SRC = \
	CdrInterface.cxx \
	CdrData.cxx


#OBJS=${SRC:.cxx=.o}

#all: ${TARGET_LIBRARY}

#clean:
#	rm -f ${OBJS}

include $(BUILD)/Makefile.post
