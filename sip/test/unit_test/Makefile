# $Id: Makefile,v 1.1 2004/05/01 04:15:26 greear Exp $

BUILD = ../../../build

include $(BUILD)/Makefile.pre

PACKAGES += PTHREAD SIP SDP2 SIPMGCP UTIL LIBCEXT LIBXML OPENSSL

CODE_SUBDIRS =  samples unit_test performance

CXXFLAGS += -DPERF_DEBUG	

# TODO:
#	MessageMsgTest.cxx \
#	UnknownExtMsgTest.cxx \
#	BadMsgTest.cxx \
#

VERIFY_TESTCASES = \
	InviteMsgTest.cxx \
	SubscribeMsgTest.cxx \
        RegisterMsgTest.cxx \
        AuthorizationHeaderTest.cxx \
        PresUrlTest.cxx \
        ImUrlTest.cxx \
        SipSessionExpiresTest.cxx \
	ReferMsgTest.cxx \
	SipToTest.cxx \
        CancelMsgTest.cxx \
        ByeMsgTest.cxx \
        StatusMsgTest.cxx \
        ContentDataTest.cxx \
	SipOspTest.cxx \
	SubscribeStatusMsgTest.cxx\
	SipFromTest.cxx\
	SipDigestTest.cxx\
	SipRawHeaderTest.cxx \
	SipDateTest.cxx\
	SipMsgParseVerify.cxx\
	SipUrlTest.cxx \
	UnknownUrlTest.cxx \
	SipMethodTest.cxx\
	SipRawHeaderContainerTest.cxx\
	SipHeaderListTest.cxx\
        AckMsgTest.cxx\
	SipContentDataContainerTest.cxx\
	SipTransactionIdTest.cxx\
	SimpleTransactionTest.cxx\
	SipSdpTest.cxx \
        LazyGetTest.cxx \
        CancelMsgTest.cxx \
        ByeMsgTest.cxx \
	TestTransactionId.cxx\
	NotifyMsgTest.cxx\
        HostNameCompTest.cxx \
        SipBasicTest.cxx \
        SipMsgLength.cxx \
	SipCallLegTest.cxx  \
	SipCSeqTest.cxx\
	SipContentTypeTest.cxx\
	SipContentDispositionTest.cxx\
	SipViaTest.cxx\
	TelUrlTest.cxx\
	SipContactTest.cxx\
	BaseUrlTest.cxx\
	SipParameterListTest.cxx\
	SipExpiresTest.cxx\
	SipTcpFundamentalsTest.cxx\
	SipAcceptTest.cxx\
	SipAcceptLanguageTest.cxx\
	SipCallIdTest.cxx\
	SipSubsNotifyEventTest.cxx\
	SipProxyAuthorizationTest.cxx\
	SipAuthorizationTest.cxx\
	SipAuthSystem.cxx\
	SipUserAgentTest.cxx\
	SipRequestLineTest.cxx\
	SipMsgTest.cxx

#	SipAcceptEncodingTest.cxx\


SRC = \


include $(BUILD)/Makefile.post
