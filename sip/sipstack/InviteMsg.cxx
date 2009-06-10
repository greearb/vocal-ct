/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

static const char* const InviteMsg_cxx_Version =
    "$Id: InviteMsg.cxx,v 1.2 2004/05/04 07:31:14 greear Exp $";


#include "Data.hxx"
#include "InviteMsg.hxx"
#include "SipAuthorization.hxx"
#include "SipContact.hxx"
#include "SipOspAuthorization.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipSdp.hxx"
#include "SipServer.hxx"
#include "SipSubject.hxx"
#include "SipUnsupported.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"

using namespace Vocal;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpConnection;


InviteMsg::InviteMsg(const string& local_ip)
        : SipCommand(local_ip, "InviteMsg")
{
    myRequestLine.setMethod(INVITE_METHOD);
    SipCSeq cseq( SIP_INVITE, Data(int(0)), local_ip );
    setCSeq( cseq );
}



InviteMsg::InviteMsg(const InviteMsg& src, Sptr <BaseUrl> url, const string& local_ip)
    : SipCommand(local_ip, "InviteMsg")
{
    *this = src;

    myRequestLine.setUrl(url);
    //create the callId header
    
    setCallId(SipCallId("", local_ip));
}

InviteMsg::InviteMsg(const InviteMsg& src)
        : SipCommand(src)
{
    myRequestLine.setMethod(INVITE_METHOD);
}


InviteMsg::InviteMsg(const InviteMsg& msg, enum InviteMsgForward)
    : SipCommand(msg)
{
    *this = msg;

    Sptr <BaseUrl> dest = getTo().getUrl();

    int numRoute = getNumRoute();

    if ( numRoute )
    {
        dest = getRoute(0).getUrl();
        //remove that route.
        removeRoute(0);
    }
    else
    {
        dest = getTo().getUrl();
    }
    myRequestLine.setUrl(dest);
}


///
InviteMsg&
InviteMsg::operator=( const InviteMsg& src )
{
    if (&src != this )
    {
        SipCommand::operator=( src );

    }

    return ( *this );
}


bool 
InviteMsg::operator ==(const InviteMsg& src) const
{
    return SipCommand::operator==(src);
}


InviteMsg::InviteMsg(const StatusMsg& statusMsg, const string& local_ip)
    : SipCommand(statusMsg, local_ip, "InviteMsg")
{
}


/// UAS uses
InviteMsg::InviteMsg(const SipCommand& src,
                     const SipVia& via,
                     const SipCSeq& cseq)
    : SipCommand(src, via, cseq)
{
}


InviteMsg::InviteMsg(Sptr <BaseUrl> toUrl, const string& local_ip,
                     int listenPort, int rtpPort)
        : SipCommand(local_ip, "InviteMsg")
{
    assert(myMimeList.getNumContentData() == 0);
    cpLog(LOG_DEBUG_STACK, "C'tor of Invite");

    if (toUrl.getPtr() != 0)
    {
	cpLog(LOG_DEBUG_STACK, "SipUrl: %s", toUrl->encode().logData());
	setInviteDetails(toUrl, local_ip, listenPort, rtpPort);
    }
}


void 
InviteMsg::setInviteDetails(Sptr <BaseUrl> toUrl, const string& local_ip,
                            int listenPort, int rtpPort)
{
    if (toUrl.getPtr() != 0)
    {
	cpLog(LOG_DEBUG_STACK, "SipUrl:setInviteDetails: %s", toUrl->encode().logData());
    }
    //build and store in sipMessage.
    //build the non-SDP part here, and call SdpMsg class to build the SDP part.

    //build each header , and store it in that header class.
    cpLog(LOG_DEBUG_STACK, "In InviteMsg");

    //create the RequestLine.
    SipRequestLine siprequestline(SIP_INVITE, toUrl, getLocalIp());
    setRequestLine(siprequestline);

    cpLog(LOG_DEBUG_STACK, "Completed the requestLine");

    //create the Via header.
    SipVia sipvia("", getLocalIp());

    sipvia.setHost(getLocalIp());
    //The default port number is set automatically.

    Data newport(listenPort);
    if (newport.length() > 0)
    {
        sipvia.setPort(newport.convertInt());
    }

    setVia(sipvia, 0);

    cpLog(LOG_DEBUG_STACK, "Completed the via header");

    //create the From header
    SipFrom sipfrom("", getLocalIp());
    //getUserName, and getDisplayName are extern functions,
    //which contains the user's details. Set from command line?
    Data user = theSystem.getUserName();
    sipfrom.setUser(user);
    Data dispName = theSystem.getDisplayName();
    sipfrom.setDisplayName(dispName);
    Data port(listenPort);
    sipfrom.setHost( Data(local_ip) );
    sipfrom.setPort(port);
    setFrom(sipfrom);

    cpLog(LOG_DEBUG_STACK, "Completed the from header");


    //create the To header
    Sptr <BaseUrl> requestUrl = siprequestline.getUrl();

    SipTo sipto(requestUrl, getLocalIp());
    if (toUrl.getPtr() != 0)
    {
	Data name = toUrl->getUserValue();
	sipto.setDisplayName(name);
    }
    setTo(sipto);

    cpLog(LOG_DEBUG_STACK, "Completed the to header");

    //create the callId header
    SipCallId callId("", local_ip);
    setCallId(callId);

    cpLog(LOG_DEBUG_STACK, "Completed the call id header");

    //create the CSeq header

    Data cseqval = "1";
    SipCSeq cseq(SIP_INVITE, cseqval, getLocalIp());
    SipMsg::setCSeq(cseq);

    //create the subject.
    SipSubject subject("VovidaINVITE", getLocalIp());
    setSubject(subject);

    //set Contact.  This is MUST in Invite.
    SipContact contact("", getLocalIp());
    contact.setUrl(sipfrom.getUrl());
    contact.setDisplayName(sipfrom.getDisplayName());
    setContact(contact);
   

    
    SipContentType type(Data("application"), Data("SDP"), getLocalIp() );
    setContentType(type);
    //create the sdp.
    SipSdp sdp(getLocalIp());
    //Change this so that we call functions in SDP, and SDP abstracts this
    //from us.
    // set connection details.
    SdpSession sdpDesc;

    Data hostAddr = local_ip;
    SDP::AddressType protocol = SDP::AddressTypeIPV4;

    SdpConnection connection;
    LocalScopeAllocator lo;
    connection.setUnicast(hostAddr.getData(lo));
    
    // There is probably a better way to do this...
    if(hostAddr.find(":") != Data::npos)
	    protocol = SDP::AddressTypeIPV6;

    connection.setAddressType(protocol);

    sdpDesc.setConnection(connection, "setInviteDetails");

    SdpMedia* media = new SdpMedia;
    media->setPort(rtpPort);

    sdpDesc.addMedia(media);
    srandom((unsigned int)time(NULL));
    int verStr = random();

    srandom((unsigned int)time(NULL));
    int sessionStr = random();

    sdpDesc.setVersion(verStr);
    sdpDesc.setSessionId(sessionStr);

    //To further perform any other operation on that particular media
    //item, use loc.
    sdp.setSdpDescriptor(sdpDesc);
    sdp.setUserName(theSystem.getUserName());

    sdp.setSessionName(theSystem.getSessionName());
    sdp.setAddress(hostAddr);
    sdp.setAddressType(protocol);
    setContentData(&sdp, 0);
}


InviteMsg::InviteMsg( const Data& data, const string& local_ip ) throw (SipParserException&)
        : SipCommand(local_ip, "InviteMsg")
{
    try
    {
        SipCommand::decode(data);
    }
    catch (SipParserException& e)
    {
        cpLog(LOG_ERR, "Failed to parse the SIP msg, since: %s", 
              e.getDescription().c_str());
        throw SipParserException(e.getDescription(),
                                 __FILE__,
                                 __LINE__);
    }
}    // InviteMsg::InviteMsg( Data& data )


Method InviteMsg::getType() const
{
    return SIP_INVITE;
}

const SipSessionExpires& InviteMsg::getSessionExpires(const string& local_ip) const
{
    Sptr<SipSessionExpires> x;
    myHeaderList.getParsedHeader(x, SIP_SESSION_EXPIRES_HDR, local_ip);
    return *x;
}

const SipMinSE& InviteMsg::getMinSE(const string& local_ip) const 
{
    Sptr<SipMinSE> x;
    myHeaderList.getParsedHeader(x, SIP_MIN_SE_HDR, local_ip);
    return *x;
}

void InviteMsg::setSessionExpires(const SipSessionExpires& sess, const string& local_ip)
{
    Sptr<SipSessionExpires> x;
    myHeaderList.getParsedHeader(x, SIP_SESSION_EXPIRES_HDR, local_ip);
    *x = sess;
}
void InviteMsg::setMinSE(const SipMinSE& minSe, const string& local_ip)
{
    Sptr<SipMinSE> x;
    myHeaderList.getParsedHeader(x, SIP_MIN_SE_HDR, local_ip);
    *x = minSe;
}

