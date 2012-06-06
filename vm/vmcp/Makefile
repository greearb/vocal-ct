# $Id: Makefile,v 1.4 2006/03/12 07:41:31 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += UTIL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ \
	           -L$(ROOT)/lib.$(TARGET_NAME)/

MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lvutil

ifeq ($(ISMINGW),1)
MKSHLIB_LIBS += -lws2_32 -lwinmm -lvutil
LDLIBS_LAST += -lws2_32 -lwinmm -lvutil
endif


#INCDIRS += codec

TARGET_LIBRARY = libvmcp

TESTPROGRAMS = \
	MiniVmcp.cxx

# name of all files to be compiled (don't include header files)
SRC = \
	Vmcp.cxx \
	Streamio.cxx \
	Lineparser.cxx

include $(BUILD)/Makefile.post
