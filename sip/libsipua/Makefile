# $Id: Makefile,v 1.1 2004/05/01 04:15:25 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

TARGET_LIBRARY = libsipua

#PACKAGES += PROXYBASE SIP

PACKAGES += PROXYBASE RADIUS SIP SDP2 PTHREAD UTIL LIBXML

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ -L../sipstack/obj.$(TARGET_NAME)/ \
   -L$(ROOT)/sdp/sdp2/obj.$(TARGET_NAME)/  -L$(ROOT)/sip/base/obj.$(TARGET_NAME)/ 
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lpthread -lstdc++ -lm -lgcc_s -lvutil -lsip -lsdp2 -lproxyBase


INCDIRS += $(ROOT)/sip/libsipua  $(ROOT)/sip/libsipua/uaState $(ROOT)/sip/libsipua/controlState

CODE_SUBDIRS =  uaState controlState


TESTPROGRAMS = 

SRC = \
BasicAgent.cxx \
SipCallLegData.cxx \
MultiLegCallData.cxx \
SipTransactionPeers.cxx \
CallDB.cxx \
UaBase.cxx \
UaServer.cxx \
UaClient.cxx \
ContactData.cxx \
uaState/UaState.cxx \
uaState/UaStateEnd.cxx \
uaState/UaStateFailure.cxx \
uaState/UaStateIdle.cxx \
uaState/UaStateInCall.cxx \
uaState/UaStateRinging.cxx \
uaState/UasStateTrying.cxx \
uaState/UacStateTrying.cxx \
uaState/UaStateFactory.cxx \
uaState/UaStateRedirect.cxx \
uaState/UaStateInHold.cxx \
CallControl.cxx 


#docs:	doc++/config.txt
#	doc++ --config doc++/config.txt


EXTRA_HEADERS= \
uaState/UaState.hxx \
uaState/UaStateEnd.hxx \
uaState/UaStateFailure.hxx \
uaState/UaStateIdle.hxx \
uaState/UaStateInCall.hxx \
uaState/UaStateRinging.hxx \
uaState/UasStateTrying.hxx \
uaState/UacStateTrying.hxx \
uaState/UaStateFactory.hxx \
uaState/UaStateRedirect.hxx \
uaState/UaStateInHold.hxx 

include $(BUILD)/Makefile.post
