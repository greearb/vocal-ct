# $Id: Makefile,v 1.9 2006/03/14 18:05:22 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += SDP2 UTIL LIBXML

MKSHLIB_INCDIRS += -L$(ROOT)/util/obj.$(TARGET_NAME)/ -L$(ROOT)/sdp/sdp2/obj.$(TARGET_NAME)/
#MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lgcc_s -lvutil -lsdp2
MKSHLIB_LIBS += $(MKSHLIB_INCDIRS) -lstdc++ -lm -lvutil -lsdp2

TARGET_LIBRARY = libsip

#INCDIRS += $(ROOT)/util .. $(ROOT)/rtp
#INCDIRS += 

#CXXFLAGS += -Wno-unused
#TESTPROGRAMS = Invitetest.cxx

# here is the thing to enable the SIP filter
#CXXFLAGS += -DUSE_VFILTER

SRC = \
    EmbeddedObj.cxx \
    AckMsg.cxx\
    BaseUrl.cxx\
    BaseUser.cxx\
    ByeMsg.cxx\
    CancelMsg.cxx\
    EmbeddedObj.cxx \
    ImUrl.cxx \
    ImpBaseUrl.cxx \
    InfoMsg.cxx\
    InviteMsg.cxx\
    MessageMsg.cxx\
    NotifyMsg.cxx\
    NullUrl.cxx\
    NullUser.cxx\
    OptionsMsg.cxx\
    ReferMsg.cxx\
    RegisterMsg.cxx\
    SessionTimer.cxx \
    SipAccept.cxx\
    SipAcceptEncoding.cxx\
    SipAcceptLanguage.cxx\
    SipAllow.cxx\
    SipAlso.cxx\
    SipAuthorization.cxx\
    SipBasic.cxx\
    SipCHAP.cxx\
    SipCSeq.cxx\
    SipCallId.cxx\
    SipCallLeg.cxx\
    SipCommand.cxx\
    SipContact.cxx\
    SipContentData.cxx\
    SipContentDataContainer.cxx \
    SipContentDisposition.cxx\
    SipContentEncoding.cxx\
    SipContentLanguage.cxx \
    SipContentLength.cxx\
    SipContentType.cxx\
    SipDate.cxx\
    SipDigest.cxx\
    SipDiversion.cxx\
    SipEncryption.cxx\
    SipExpires.cxx\
    SipFragData.cxx\
    SipFrom.cxx\
    SipFtrSupportedEntry.cxx\
    SipFtrSupportedTable.cxx\
    SipHeader.cxx\
    SipHide.cxx\
    SipInReplyTo.cxx \
    SipIsup.cxx\
    SipMaxForwards.cxx\
    SipMethod.cxx \
    SipMimeVersion.cxx \
    SipMinSE.cxx \
    SipMsg.cxx\
    SipMsgQueue.cxx\
    SipOrganization.cxx\
    SipOsp.cxx\
    SipOspAuthorization.cxx \
    SipParameterList.cxx \
    SipParserMode.cxx\
    SipPriority.cxx\
    SipProxyAuthenticate.cxx\
    SipProxyAuthorization.cxx\
    SipProxyRequire.cxx\
    SipRawHeader.cxx\
    SipRawHeaderContainer.cxx\
    SipRecordRoute.cxx\
    SipReferTo.cxx\
    SipReferredBy.cxx\
    SipReplaces.cxx\
    SipRequestLine.cxx\
    SipRequire.cxx\
    SipResponseKey.cxx\
    SipRetryAfter.cxx\
    SipRoute.cxx\
    SipSdp.cxx\
    SipSentRequestDB.cxx \
    SipSentResponseDB.cxx \
    SipServer.cxx\
    SipSession.cxx \
    SipSessionExpires.cxx \
    SipStatusLine.cxx\
    SipSubject.cxx\
    SipSubsNotifyEvent.cxx\
    SipSupported.cxx \
    SipTcpConnection.cxx\
    SipTelSubscriberUser.cxx\
    SipTextData.cxx \
    SipTimestamp.cxx\
    SipTo.cxx\
    SipTransactionDB.cxx \
    SipTransactionId.cxx \
    SipMsgContainer.cxx \
    SipTransceiver.cxx\
    SipTransferTo.cxx\
    SipUdpConnection.cxx\
    SipUnknownContentData.cxx\
    SipUnsupported.cxx\
    SipUriSupportedEntry.cxx\
    SipUriSupportedTable.cxx\
    SipUrl.cxx\
    SipUser.cxx\
    SipUserAgent.cxx\
    SipVector.cxx\
    SipVia.cxx\
    SipWarning.cxx\
    SipWwwAuthenticate.cxx\
    SnmpData.cxx\
    StatusMsg.cxx\
    SubscribeMsg.cxx\
    SystemInfo.cxx\
    TelUrl.cxx\
    TransferMsg.cxx\
    UnknownExtensionMsg.cxx \
    UnknownUrl.cxx\
    symbols.cxx \
    PresUrl.cxx \

include $(BUILD)/Makefile.post
