# $Id: Makefile,v 1.2 2004/06/22 02:24:04 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += UTIL LIBXML OPENSSL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lgcc_s -lvutil -lxml


#INCDIRS += codec

TARGET_LIBRARY = libvmcp

TESTPROGRAMS = \
	MiniVmcp.cxx

# name of all files to be compiled (don't include header files)
SRC = \
	Vmcp.cxx \
	VmTime.cxx \
	Streamio.cxx \
	Lineparser.cxx

include $(BUILD)/Makefile.post
