# $Id: Makefile,v 1.1 2004/05/01 04:15:38 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += UTIL LIBXML PTHREAD OPENSSL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lpthread -lstdc++ -lm -lgcc_s -lvutil -lxml


#INCDIRS += codec

#DEFINES += USE_PTHREADS

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
