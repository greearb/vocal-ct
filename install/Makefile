BUILD = ../build

include $(BUILD)/Makefile.pre

CXXFLAGS += -DUSE_VFILTER

ifeq ($(OSTYPE),SunOS)
OS_SOLARIS=1
DEFINES += OS_SOLARIS
endif


TESTPROGRAMS = \
	deploy.cxx \

include $(BUILD)/Makefile.post
