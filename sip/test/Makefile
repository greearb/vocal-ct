# $Id: Makefile,v 1.1 2004/05/01 04:15:26 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += PTHREAD SIP SDP2 SIPMGCP UTIL LIBCEXT LIBXML OPENSSL

CODE_SUBDIRS =  samples

CXXFLAGS += -DPERF_DEBUG	

TESTPROGRAMS = \
	200AckUAC.cxx \
	200AckUAS.cxx \
	EmbeddedObjTest.cxx \
	DigestCalculator.cxx \
        InfoMsgTest.cxx \
        SipDataTest.cxx \
        SessionTimerTest.cxx \
        SessionTimerTestRecv.cxx \
	chapterExample.cxx\
        sendInParallel.cxx \
        recvInParallel.cxx \
	ParseUnparseMsg.cxx  \
	RecvPrint.cxx \
	ExpandTest.cxx \
	Invitetest.cxx \
        ReceiveSIPTest.cxx \
        SendSIPTest.cxx \
	InviteCancelSend.cxx \
	InviteCancelReceive.cxx \
	RecordRouteMem.cxx \
	FullCallSend.cxx\
	FullCallReceive.cxx\
	ParseFile.cxx \
	ReceiveTimeTest.cxx\
	SendTimeTest.cxx\
	parseOneMsg.cxx \
	stringProfTest.cxx \
	CopyCallId.cxx\
	DecodeMultiple.cxx\
	ReceiveDropFloor.cxx\
	SampleStatisticUse.cxx\
	ParseSingle.cxx\
	FullParseOneMsg.cxx\
	MicroProxy.cxx\
	ParseContentData.cxx\
	ParseUnknownMethod.cxx\
	minimal.cxx\
	TestThread.cxx\
	SimpleSendingExample.cxx\
	SimpleReceivingExample.cxx\
	Datatest.cxx\
	SipComputeBranchTest.cxx


TESTPROGRAMS += \
	samples/3pcc.cxx



# TODO:
#	MessageMsgTest.cxx \
#	UnknownExtMsgTest.cxx \
#	BadMsgTest.cxx \
#

SRC = \


include $(BUILD)/Makefile.post
