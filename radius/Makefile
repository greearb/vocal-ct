# $Id: Makefile,v 1.1 2004/05/01 04:15:22 greear Exp $

BUILD = ../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libradius

PACKAGES += PTHREAD UTIL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lpthread -lstdc++ -lm -lgcc_s -lvutil


SRC = \
	RadiusAttribute.cxx \
	RadiusMessage.cxx

include $(BUILD)/Makefile.post
