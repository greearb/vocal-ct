# $Id: Makefile,v 1.2 2004/06/22 02:24:04 greear Exp $

BUILD = ../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libradius

PACKAGES += UTIL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lgcc_s -lvutil


SRC = \
	RadiusAttribute.cxx \
	RadiusMessage.cxx

include $(BUILD)/Makefile.post
