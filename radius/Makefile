# $Id: Makefile,v 1.3 2004/11/19 01:54:38 greear Exp $

BUILD = ../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libradius

PACKAGES += UTIL

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lvutil


SRC = \
	RadiusAttribute.cxx \
	RadiusMessage.cxx

include $(BUILD)/Makefile.post
