# $Id: Makefile,v 1.3 2004/11/19 01:54:38 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += UTIL LIBXML OPENSSL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ \
	           -L$(ROOT)/lib.$(TARGET_NAME)/

MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lvutil -lvxml


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
