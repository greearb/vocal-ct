# Microsoft Developer Studio Project File - Name="sip" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sip - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sip.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sip.mak" CFG="sip - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sip - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sip - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sip - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "obj.nodebug.Win32.i686"
# PROP BASE Intermediate_Dir "obj.nodebug.Win32.i686"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj.nodebug.Win32.i686"
# PROP Intermediate_Dir "obj.nodebug.Win32.i686"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\build" /I "." /I "..\..\util" /I "..\..\util\threads" /I "..\..\util\logging" /I "..\..\util\crypto" /I "..\..\util\statistics" /I "..\..\util\snmp" /I "..\..\util\signals" /I "..\..\util\behavior" /I "..\..\util\io" /I "..\..\util\services" /I "..\..\util\transport" /I "..\..\util\config" /I "..\..\util\dnssrv" /I "..\..\util\deprecated" /I "..\..\sdp" /I "..\..\contrib\win32" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\libxml" /I "..\..\contrib\win32\pthreads" /I "..\..\contrib\libcext_bsd" /I "..\..\contrib\win32\regex" /I "..\..\contrib\win32\getopt" /I "..\..\sdp\sdp2" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "sip - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "obj.debug.Win32.i686"
# PROP BASE Intermediate_Dir "obj.debug.Win32.i686"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj.debug.Win32.i686"
# PROP Intermediate_Dir "obj.debug.Win32.i686"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\build" /I "." /I "..\..\util" /I "..\..\util\threads" /I "..\..\util\logging" /I "..\..\util\crypto" /I "..\..\util\statistics" /I "..\..\util\snmp" /I "..\..\util\signals" /I "..\..\util\behavior" /I "..\..\util\io" /I "..\..\util\services" /I "..\..\util\transport" /I "..\..\util\config" /I "..\..\util\dnssrv" /I "..\..\util\deprecated" /I "..\..\sdp" /I "..\..\contrib\win32" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\libxml" /I "..\..\contrib\win32\pthreads" /I "..\..\contrib\libcext_bsd" /I "..\..\contrib\win32\regex" /I "..\..\contrib\win32\getopt" /I "..\..\sdp\sdp2" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "sip - Win32 Release"
# Name "sip - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AckMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\BaseUrl.cxx
# End Source File
# Begin Source File

SOURCE=.\BaseUser.cxx
# End Source File
# Begin Source File

SOURCE=.\ByeMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\EmbeddedObj.cxx
# End Source File
# Begin Source File

SOURCE=.\CancelMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\ImpBaseUrl.cxx
# End Source File
# Begin Source File

SOURCE=.\ImUrl.cxx
# End Source File
# Begin Source File

SOURCE=.\InfoMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\InviteMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\MessageMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\NotifyMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\NullUrl.cxx
# End Source File
# Begin Source File

SOURCE=.\NullUser.cxx
# End Source File
# Begin Source File

SOURCE=.\OptionsMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\PresUrl.cxx
# End Source File
# Begin Source File

SOURCE=.\ReferMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\RegisterMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\RetransmitContents.cxx
# End Source File
# Begin Source File

SOURCE=.\SipAccept.cxx
# End Source File
# Begin Source File

SOURCE=.\SipAcceptEncoding.cxx
# End Source File
# Begin Source File

SOURCE=.\SipAcceptLanguage.cxx
# End Source File
# Begin Source File

SOURCE=.\SipAgent.cxx
# End Source File
# Begin Source File

SOURCE=.\SipAllow.cxx
# End Source File
# Begin Source File

SOURCE=.\SipAlso.cxx
# End Source File
# Begin Source File

SOURCE=.\SipAuthorization.cxx
# End Source File
# Begin Source File

SOURCE=.\SipBasic.cxx
# End Source File
# Begin Source File

SOURCE=.\SipCallId.cxx
# End Source File
# Begin Source File

SOURCE=.\SipCallLeg.cxx
# End Source File
# Begin Source File

SOURCE=.\SipCommand.cxx
# End Source File
# Begin Source File

SOURCE=.\SipContact.cxx
# End Source File
# Begin Source File

SOURCE=.\SipContentData.cxx
# End Source File
# Begin Source File

SOURCE=.\SipContentDataContainer.cxx
# End Source File
# Begin Source File

SOURCE=.\SipContentDisposition.cxx
# End Source File
# Begin Source File

SOURCE=.\SipContentEncoding.cxx
# End Source File
# Begin Source File

SOURCE=.\SipContentLanguage.cxx
# End Source File
# Begin Source File

SOURCE=.\SipContentLength.cxx
# End Source File
# Begin Source File

SOURCE=.\SipContentType.cxx
# End Source File
# Begin Source File

SOURCE=.\SipCSeq.cxx
# End Source File
# Begin Source File

SOURCE=.\SipDate.cxx
# End Source File
# Begin Source File

SOURCE=.\SipDebuggingInterface.cxx
# End Source File
# Begin Source File

SOURCE=.\SipDigest.cxx
# End Source File
# Begin Source File

SOURCE=.\SipDiversion.cxx
# End Source File
# Begin Source File

SOURCE=.\SipEncryption.cxx
# End Source File
# Begin Source File

SOURCE=.\SipExpires.cxx
# End Source File
# Begin Source File

SOURCE=.\SipFragData.cxx
# End Source File
# Begin Source File

SOURCE=.\SipFrom.cxx
# End Source File
# Begin Source File

SOURCE=.\SipFtrSupportedEntry.cxx
# End Source File
# Begin Source File

SOURCE=.\SipFtrSupportedTable.cxx
# End Source File
# Begin Source File

SOURCE=.\SipHeader.cxx
# End Source File
# Begin Source File

SOURCE=.\SipHide.cxx
# End Source File
# Begin Source File

SOURCE=.\SipInReplyTo.cxx
# End Source File
# Begin Source File

SOURCE=.\SipIsup.cxx
# End Source File
# Begin Source File

SOURCE=.\SipMaxForwards.cxx
# End Source File
# Begin Source File

SOURCE=.\SipMethod.cxx
# End Source File
# Begin Source File

SOURCE=.\SipMimeVersion.cxx
# End Source File
# Begin Source File

SOURCE=.\SipMinSE.cxx
# End Source File
# Begin Source File

SOURCE=.\SipMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\SipMsgQueue.cxx
# End Source File
# Begin Source File

SOURCE=.\SipOrganization.cxx
# End Source File
# Begin Source File

SOURCE=.\SipOsp.cxx
# End Source File
# Begin Source File

SOURCE=.\SipOspAuthorization.cxx
# End Source File
# Begin Source File

SOURCE=.\SipParameterList.cxx
# End Source File
# Begin Source File

SOURCE=.\SipParserMode.cxx
# End Source File
# Begin Source File

SOURCE=.\SipPriority.cxx
# End Source File
# Begin Source File

SOURCE=.\SipProxyAuthenticate.cxx
# End Source File
# Begin Source File

SOURCE=.\SipProxyAuthorization.cxx
# End Source File
# Begin Source File

SOURCE=.\SipProxyRequire.cxx
# End Source File
# Begin Source File

SOURCE=.\SipRawHeader.cxx
# End Source File
# Begin Source File

SOURCE=.\SipRawHeaderContainer.cxx
# End Source File
# Begin Source File

SOURCE=.\SipRecordRoute.cxx
# End Source File
# Begin Source File

SOURCE=.\SipReferredBy.cxx
# End Source File
# Begin Source File

SOURCE=.\SipReferTo.cxx
# End Source File
# Begin Source File

SOURCE=.\SipReplaces.cxx
# End Source File
# Begin Source File

SOURCE=.\SipRequestLine.cxx
# End Source File
# Begin Source File

SOURCE=.\SipRequire.cxx
# End Source File
# Begin Source File

SOURCE=.\SipResponseKey.cxx
# End Source File
# Begin Source File

SOURCE=.\SipRetryAfter.cxx
# End Source File
# Begin Source File

SOURCE=.\SipRoute.cxx
# End Source File
# Begin Source File

SOURCE=.\SipSdp.cxx
# End Source File
# Begin Source File

SOURCE=.\SipSentRequestDB.cxx
# End Source File
# Begin Source File

SOURCE=.\SipSentResponseDB.cxx
# End Source File
# Begin Source File

SOURCE=.\SipServer.cxx
# End Source File
# Begin Source File

SOURCE=.\SipSession.cxx
# End Source File
# Begin Source File

SOURCE=.\SipSessionExpires.cxx
# End Source File
# Begin Source File

SOURCE=.\SipSnmpDetails.cxx
# End Source File
# Begin Source File

SOURCE=.\SipStatusLine.cxx
# End Source File
# Begin Source File

SOURCE=.\SipSubject.cxx
# End Source File
# Begin Source File

SOURCE=.\SipSubsNotifyEvent.cxx
# End Source File
# Begin Source File

SOURCE=.\SipSupported.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTcpConnection.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTelSubscriberUser.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTextData.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTimestamp.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTo.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionDB.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionGC.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionId.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionLevels.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTransceiver.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTransDebugger.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTransferTo.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTransHashTable.cxx
# End Source File
# Begin Source File

SOURCE=.\SipUdp_impl.cxx
# End Source File
# Begin Source File

SOURCE=.\SipUdpConnection.cxx
# End Source File
# Begin Source File

SOURCE=.\SipUnknownContentData.cxx
# End Source File
# Begin Source File

SOURCE=.\SipUnsupported.cxx
# End Source File
# Begin Source File

SOURCE=.\SipUriSupportedEntry.cxx
# End Source File
# Begin Source File

SOURCE=.\SipUriSupportedTable.cxx
# End Source File
# Begin Source File

SOURCE=.\SipUrl.cxx
# End Source File
# Begin Source File

SOURCE=.\SipUser.cxx
# End Source File
# Begin Source File

SOURCE=.\SipUserAgent.cxx
# End Source File
# Begin Source File

SOURCE=.\SipVector.cxx
# End Source File
# Begin Source File

SOURCE=.\SipVia.cxx
# End Source File
# Begin Source File

SOURCE=.\SipWarning.cxx
# End Source File
# Begin Source File

SOURCE=.\SipWwwAuthenticate.cxx
# End Source File
# Begin Source File

SOURCE=.\SnmpData.cxx
# End Source File
# Begin Source File

SOURCE=.\StatusMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\SubscribeMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\symbols.cxx
# End Source File
# Begin Source File

SOURCE=.\SystemInfo.cxx
# End Source File
# Begin Source File

SOURCE=.\TelUrl.cxx
# End Source File
# Begin Source File

SOURCE=.\TransferMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\UnknownExtensionMsg.cxx
# End Source File
# Begin Source File

SOURCE=.\UnknownUrl.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AckMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\BaseUrl.hxx
# End Source File
# Begin Source File

SOURCE=.\BaseUser.hxx
# End Source File
# Begin Source File

SOURCE=.\ByeMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\CancelMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\InfoMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\InviteMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\MessageMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\NotifyMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\NullUrl.hxx
# End Source File
# Begin Source File

SOURCE=.\NullUser.hxx
# End Source File
# Begin Source File

SOURCE=.\OptionsMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\ReferMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\RegisterMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\RetransmitContents.hxx
# End Source File
# Begin Source File

SOURCE=.\SipAccept.hxx
# End Source File
# Begin Source File

SOURCE=.\SipAcceptEncoding.hxx
# End Source File
# Begin Source File

SOURCE=.\SipAcceptLanguage.hxx
# End Source File
# Begin Source File

SOURCE=.\SipAgent.hxx
# End Source File
# Begin Source File

SOURCE=.\SipAllow.hxx
# End Source File
# Begin Source File

SOURCE=.\EmbeddedObj.hxx
# End Source File
# Begin Source File

SOURCE=.\SipAlso.hxx
# End Source File
# Begin Source File

SOURCE=.\SipAuthorization.hxx
# End Source File
# Begin Source File

SOURCE=.\SipBaseTransactionId.hxx
# End Source File
# Begin Source File

SOURCE=.\SipBasic.hxx
# End Source File
# Begin Source File

SOURCE=.\SipCallId.hxx
# End Source File
# Begin Source File

SOURCE=.\SipCallLeg.hxx
# End Source File
# Begin Source File

SOURCE=.\SipCommand.hxx
# End Source File
# Begin Source File

SOURCE=.\SipContact.hxx
# End Source File
# Begin Source File

SOURCE=.\SipContentData.hxx
# End Source File
# Begin Source File

SOURCE=.\SipContentDataContainer.hxx
# End Source File
# Begin Source File

SOURCE=.\SipContentDisposition.hxx
# End Source File
# Begin Source File

SOURCE=.\SipContentEncoding.hxx
# End Source File
# Begin Source File

SOURCE=.\SipContentFunction.hxx
# End Source File
# Begin Source File

SOURCE=.\SipContentLanguage.hxx
# End Source File
# Begin Source File

SOURCE=.\SipContentLength.hxx
# End Source File
# Begin Source File

SOURCE=.\SipContentType.hxx
# End Source File
# Begin Source File

SOURCE=.\SipCSeq.hxx
# End Source File
# Begin Source File

SOURCE=.\SipDate.hxx
# End Source File
# Begin Source File

SOURCE=.\SipDebuggingInterface.hxx
# End Source File
# Begin Source File

SOURCE=.\SipDigest.hxx
# End Source File
# Begin Source File

SOURCE=.\SipDiversion.hxx
# End Source File
# Begin Source File

SOURCE=.\SipEncryption.hxx
# End Source File
# Begin Source File

SOURCE=.\SipExpires.hxx
# End Source File
# Begin Source File

SOURCE=.\SipFragData.hxx
# End Source File
# Begin Source File

SOURCE=.\SipFrom.hxx
# End Source File
# Begin Source File

SOURCE=.\SipFtrSupportedEntry.hxx
# End Source File
# Begin Source File

SOURCE=.\SipFtrSupportedTable.hxx
# End Source File
# Begin Source File

SOURCE=.\SipHeader.hxx
# End Source File
# Begin Source File

SOURCE=.\SipHeaderList.hxx
# End Source File
# Begin Source File

SOURCE=.\SipHide.hxx
# End Source File
# Begin Source File

SOURCE=.\SipInReplyTo.hxx
# End Source File
# Begin Source File

SOURCE=.\SipIsup.hxx
# End Source File
# Begin Source File

SOURCE=.\SipMaxForwards.hxx
# End Source File
# Begin Source File

SOURCE=.\SipMethod.hxx
# End Source File
# Begin Source File

SOURCE=.\SipMimeVersion.hxx
# End Source File
# Begin Source File

SOURCE=.\SipMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\SipMsgQueue.hxx
# End Source File
# Begin Source File

SOURCE=.\SipOrganization.hxx
# End Source File
# Begin Source File

SOURCE=.\SipOsp.hxx
# End Source File
# Begin Source File

SOURCE=.\SipOspAuthorization.hxx
# End Source File
# Begin Source File

SOURCE=.\SipParameterList.hxx
# End Source File
# Begin Source File

SOURCE=.\SipParserMode.hxx
# End Source File
# Begin Source File

SOURCE=.\SipPriority.hxx
# End Source File
# Begin Source File

SOURCE=.\SipProxyAuthenticate.hxx
# End Source File
# Begin Source File

SOURCE=.\SipProxyAuthorization.hxx
# End Source File
# Begin Source File

SOURCE=.\SipProxyRequire.hxx
# End Source File
# Begin Source File

SOURCE=.\SipRawHeader.hxx
# End Source File
# Begin Source File

SOURCE=.\SipRawHeaderContainer.hxx
# End Source File
# Begin Source File

SOURCE=.\SipReceive.hxx
# End Source File
# Begin Source File

SOURCE=.\SipReceiveStatus.hxx
# End Source File
# Begin Source File

SOURCE=.\SipRecordRoute.hxx
# End Source File
# Begin Source File

SOURCE=.\SipRedirect.hxx
# End Source File
# Begin Source File

SOURCE=.\SipReferredBy.hxx
# End Source File
# Begin Source File

SOURCE=.\SipReferTo.hxx
# End Source File
# Begin Source File

SOURCE=.\SipReplaces.hxx
# End Source File
# Begin Source File

SOURCE=.\SipRequestLine.hxx
# End Source File
# Begin Source File

SOURCE=.\SipRequire.hxx
# End Source File
# Begin Source File

SOURCE=.\SipResponseKey.hxx
# End Source File
# Begin Source File

SOURCE=.\SipRetryAfter.hxx
# End Source File
# Begin Source File

SOURCE=.\SipRoute.hxx
# End Source File
# Begin Source File

SOURCE=.\SipSdp.hxx
# End Source File
# Begin Source File

SOURCE=.\SipSentRequestDB.hxx
# End Source File
# Begin Source File

SOURCE=.\SipSentResponseDB.hxx
# End Source File
# Begin Source File

SOURCE=.\SipServer.hxx
# End Source File
# Begin Source File

SOURCE=.\SipSession.hxx
# End Source File
# Begin Source File

SOURCE=.\SipSnmpDetails.hxx
# End Source File
# Begin Source File

SOURCE=.\SipStatusLine.hxx
# End Source File
# Begin Source File

SOURCE=.\SipSubject.hxx
# End Source File
# Begin Source File

SOURCE=.\SipSubsNotifyEvent.hxx
# End Source File
# Begin Source File

SOURCE=.\SipSupported.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTcpConnection.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTelSubscriberUser.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTextData.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTimestamp.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTo.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionDB.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionGC.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionId.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionLevels.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionList.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionStatus.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransceiver.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransceiverFilter.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransDebugger.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransferTo.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransHashTable.hxx
# End Source File
# Begin Source File

SOURCE=.\SipUdp_impl.hxx
# End Source File
# Begin Source File

SOURCE=.\SipUdpConnection.hxx
# End Source File
# Begin Source File

SOURCE=.\SipUnknownContentData.hxx
# End Source File
# Begin Source File

SOURCE=.\SipUnsupported.hxx
# End Source File
# Begin Source File

SOURCE=.\SipUriSupportedEntry.hxx
# End Source File
# Begin Source File

SOURCE=.\SipUriSupportedTable.hxx
# End Source File
# Begin Source File

SOURCE=.\SipUrl.hxx
# End Source File
# Begin Source File

SOURCE=.\SipUser.hxx
# End Source File
# Begin Source File

SOURCE=.\SipUserAgent.hxx
# End Source File
# Begin Source File

SOURCE=.\SipVector.hxx
# End Source File
# Begin Source File

SOURCE=.\SipVia.hxx
# End Source File
# Begin Source File

SOURCE=.\SipWarning.hxx
# End Source File
# Begin Source File

SOURCE=.\SipWwwAuthenticate.hxx
# End Source File
# Begin Source File

SOURCE=.\SnmpData.hxx
# End Source File
# Begin Source File

SOURCE=.\StatusMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\SubscribeMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\symbols.hxx
# End Source File
# Begin Source File

SOURCE=.\SystemInfo.hxx
# End Source File
# Begin Source File

SOURCE=.\TelUrl.hxx
# End Source File
# Begin Source File

SOURCE=.\TransceiverSymbols.hxx
# End Source File
# Begin Source File

SOURCE=.\TransferMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\UnknownExtensionMsg.hxx
# End Source File
# Begin Source File

SOURCE=.\UnknownUrl.hxx
# End Source File
# Begin Source File

SOURCE=.\VovidaSipStack.hxx
# End Source File
# End Group
# End Target
# End Project
