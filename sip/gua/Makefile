# $Id: Makefile,v 1.5 2004/06/20 22:28:00 greear Exp $

USE_VM=1
BUILD = ../../build
CXXFLAGS += -I../../libmedia 
LDLIBS_LAST +=  -lm

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lgcc_s -lvutil


# Voice Age Codec Support !
# 
# Uncomment the following lines if you have the VoiceAge G729 library
# CXXFLAGS += -DUSE_VOICE_AGE 
# LDLIBS_LAST += -lg729 
# NOTE: Changes are also required in libmedia/Makefile, 
#       put the library in libmedia/obj.* 


# Comment this out if you don't want speex codec support.
INCDIRS += -DUSE_SPEEX
LIBDIRS += $(ROOT)/contrib/speex.$(TARGET_NAME)/build/lib
LDLIBS_LAST += -lspeex


include $(BUILD)/Makefile.pre
ifeq ($(OSTYPE),SunOS)
PACKAGES += MEDIALIB PROXYBASE SIPUA SIP SDP2 RTP UTIL LIBCEXT LIBXML HEARTBEAT
else
PACKAGES += MEDIALIB PROXYBASE SIPUA SIP SDP2 NEWSNDFILE LIBSNDCARD RTP UTIL LIBCEXT OPENSSL PSLIB PSUTILIB LIBXML HEARTBEAT
endif

INCDIRS += $(ROOT)/sip/libsipua $(ROOT)/sip/gua/controlState \
           $(ROOT)/sip/libsipua/uaState $(ROOT)/sip/gua 

# Speex support.
INCDIRS += $(ROOT)/contrib/speex.$(TARGET_NAME)/build/include
LIBDIRS += $(ROOT)/contrib/speex.$(TARGET_NAME)/build/lib
LDLIBS_LAST += -lspeex


CODE_SUBDIRS =  controlState 

ifeq ($(USE_VM),1)
DEFINES += USE_VM 
PACKAGES += VMCP NEWSNDFILE
endif

ifeq ($(VOCAL_USE_SPHINX),true)
DEFINES += VOCAL_USE_SPHINX
PACKAGES += SPHINX
endif

ifeq ($(USE_MPEGLIB),1)
MPEG4IP_ROOT=/scratch/mpeg4ip-0.9.6
DEFINES += USE_MPEGLIB  
GTK_CFLAGS = /usr/include/gtk-1.2 /usr/include/glib-1.2 /usr/lib/glib/include /usr/X11R6/include
INCDIRS += ${GTK_CFLAGS} \
           ${MPEG4IP_ROOT}/include \
           ${MPEG4IP_ROOT}/server/mp4live  \
           ${MPEG4IP_ROOT}/server/mp4live/util  \
           ${MPEG4IP_ROOT}/server/util \
           ${MPEG4IP_ROOT}/lib/SDL/include \
           ${MPEG4IP_ROOT}/lib/msg_queue \
           ${MPEG4IP_ROOT}/lib/mp4v2 \
           ${MPEG4IP_ROOT}/lib/mp4av \
           ${MPEG4IP_ROOT}/lib \
           ${MPEG4IP_ROOT}/player/lib \
           ${MPEG4IP_ROOT}/player/src
LIBDIRS += /usr/local/lib /usr/X11R6/lib \
           ${MPEG4IP_ROOT}/server/mp4live/gui/.libs \
           ${MPEG4IP_ROOT}/server/mp4live/.libs \
           ${MPEG4IP_ROOT}/common/video/libmpeg32/.libs \
           ${MPEG4IP_ROOT}/server/audio/faac/.libs \
           ${MPEG4IP_ROOT}/server/audio/lame \
           ${MPEG4IP_ROOT}/lib/xvid/.libs  \
           ${MPEG4IP_ROOT}/lib/rtp/.libs \
           ${MPEG4IP_ROOT}/player/src/.libs \
           ${MPEG4IP_ROOT}/lib/SDL/src \
           ${MPEG4IP_ROOT}/lib/SDL/src/.libs \
           ${MPEG4IP_ROOT}/lib/SDL/main \
           ${MPEG4IP_ROOT}/lib/libhttp/


LDLIBS_LAST += \
               -Wl,--rpath /usr/local/lib \
               -lmp4livegui \
               -lmp4live \
               -lmp4player \
               -lmp4syncbase \
               -lmp4syncsdl \
               -lmp4syncbase \
               -lmp4playerutils \
               -lmp4util \
               -lmp4av  \
               -lmp4v2  \
               -lmp4 \
               -lsdp \
               -lmsg_queue \
               -lmpeg3 \
               -lmpeg3util \
               -lg729 \
	       -lfaac \
               -lmp3lame \
               -luclmmbase \
               -lxvid \
               -lhttp \
               -lconfig_file \
               -Wl,-Bstatic \
               -lSDL \
               -lSDLmain \
               -lartsc -lesd -lasound \
               -Wl,-Bdynamic \
               -lgtk \
               -lgdk \
               -lglib \
               -lgmodule \
               -lX11 -lXext -lXft -ldl 
endif


TESTPROGRAMS = \
    gua.cxx

SRC = \
CallAgent.cxx \
UaFacade.cxx \
GuiEvent.cxx \
UaCallControl.cxx \
UaConfiguration.cxx \
RegistrationManager.cxx \
Registration.cxx \
Authenticate.cxx \
UaCommandLine.cxx \
UaHardwareEvent.cxx \
controlState/ControlState.cxx \
controlState/ControlStateFactory.cxx \
controlState/StateInCall.cxx \
controlState/StateInit.cxx \
controlState/StateTrying.cxx \
controlState/StateUacRinging.cxx \
controlState/StateUasRinging.cxx \
controlState/StateInHold.cxx \
UaCli.cxx \
PlayQueue.cxx \
FileMediaDevice.cxx

ifeq ($(OSTYPE),Linux)
SRC += LinAudioDevice.cxx 
endif

ifeq ($(USE_VM),1)
SRC += VmcpDevice.cxx \
       Recorder.cxx
endif
ifeq ($(USE_MPEGLIB),1)
SRC += VideoDevice.cxx 
endif

ifeq ($(USE_MPEGLIB),1)
GUIMAKE=make -f Makefile.vid USE_MPEGLIB=1
else
GUIMAKE=make
endif

ifeq ($(VOCAL_USE_SPHINX),true)
SRC += SpeechDevice.cxx \
	SpeechHandler.cxx \
	LookupTable.cxx
endif


ifeq ($(VOCAL_USE_LANFORGE),yes)

CODE_SUBDIRS += LF

SRC += LF/LFVoipCtrlHandler.cxx LF/LFVoipEndp.cxx LF/LFVoipHelper.cxx LF/LFVoipManager.cxx \
       LF/LANforgeCustomDevice.cxx


LFDIR=$(HOME)/btbits/btbits
INCDIRS += $(LFDIR)/server $(LFDIR)/include $(LFDIR)/l4libs/curl-7.10.4/include
LIBDIRS += $(LFDIR)/lib

# Link in the LANforge .o files needed by the helper process..
LDLIBS_LAST += $(LFDIR)/server/misc.o $(LFDIR)/server/CtrlHandler.o \
               $(LFDIR)/server/gui_msgs.o $(LFDIR)/server/eid.o \
               $(LFDIR)/server/BuildInfo.o $(LFDIR)/server/iohandler.o -lz

LDLIBS_LAST += -lstring2 -lbitfield -lgcont


endif


#rules for making gui front-end

GUI = gui

GLADESRC = gui/src/support.h \
	gui/src/support.c \
	gui/src/interface.h \
	gui/src/interface.c

gui/src/sipset: 
	cd $(GUI) ; rm -f Makefile ; ./autogen.sh

gui: gui/src/sipset
	cd $(GUI) ; ./configure; cd src; $(GUIMAKE)

GUISRC = gui/src/callbacks.c \
	 gui/src/callbacks.h \
	 gui/src/configuration.h \
	 gui/src/configuration.c \
	 gui/src/main.c

include $(BUILD)/Makefile.post

