BUILD = ../../build

include $(BUILD)/Makefile.pre

USE_HEARTBEAT = $(shell if [ -d ../../heartbeat ] ; then echo 1 ; fi)

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ -L$(ROOT)/sdp/sdp2/obj.$(TARGET_NAME)/ \
  -L$(ROOT)/sip/sipstack/obj.$(TARGET_NAME)/ -L$(ROOT)/heartbeat/obj.$(TARGET_NAME)/ 
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lgcc_s -lvutil -lsdp2 -lsip -lheartbeat

ifeq ($(USE_HEARTBEAT),1)
     PACKAGES += HEARTBEAT PSLIB PSUTILIB LIBXML LIBCEXT OPENSSL
endif

PACKAGES += SIP SDP2 UTIL LIBCEXT OPENSSL

INCDIRS += .

TARGET_LIBRARY := libproxyBase
SRC = \
	Builder.cxx \
	CallContainer.cxx \
	CallInfo.cxx \
	DeviceEvent.cxx \
	SipProxyEvent.cxx \
	Feature.cxx \
	Operator.cxx \
	SipEvent.cxx \
	SipThread.cxx \
	State.cxx \
	TimerEvent.cxx \
	HeartLessProxy.cxx \
	BaseCommandLine.cxx \
	StateMachnData.cxx \
	BaseAuthentication.cxx \
	StatelessBasicProxy.cxx \
	CommonFunctions.cxx

ifeq ($(USE_HEARTBEAT),1)

SRC += 	BasicProxy.cxx

# TESTPROGRAMS +=  BaseProxyTestMain.cxx

endif

include $(BUILD)/Makefile.post

docs:   doc++/config.txt
	doc++ --config doc++/config.txt
