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

static const char* const InfoMsg_cxx_Version =
    "$Id: InfoMsg.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "cpLog.h"
#include "Data.hxx"
#include "symbols.hxx"
#include "InfoMsg.hxx"
#include "SipContact.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipSubject.hxx"
#include "SipUnsupported.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;


const char InfoMsg::INFO_SUBJECT[] = "VovidaINFO";

InfoMsg::InfoMsg(const string& local_ip): SipCommand(local_ip)
{}


InfoMsg::~InfoMsg()
{
}

InfoMsg::InfoMsg(const InfoMsg& src)
        : SipCommand(src)
{}


InfoMsg& InfoMsg::operator =(const InfoMsg& src)
{
    //copy details contained in the base class.
    if ( &src != this )
    {
        SipCommand::operator=(src);
    }
    return (*this);
}
    
bool InfoMsg::operator ==(const InfoMsg& src)
{
    return (*static_cast<SipCommand*>(this) == src );
}

InfoMsg::InfoMsg(Sptr <BaseUrl> toUrl, const string& local_ip, int fromPort)
        : SipCommand(local_ip)
{
    cpLog(LOG_DEBUG_STACK, "C'tor of Info");

    setInfoDetails(toUrl, fromPort);
}


InfoMsg::InfoMsg(const InfoMsg& msg, enum InfoMsgForward)
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

    SipRequestLine reqLine = getRequestLine();
    reqLine.setUrl( dest );
    setRequestLine( reqLine );
}

void InfoMsg::setInfoDetails(Sptr <BaseUrl> toUrl, int fromPort)
{
    //build and store in sipMessage.
    //build the non-SDP part here, and call SdpMsg class to build the SDP part.

    //build each header , and store it in that header class.
    cpLog(LOG_DEBUG_STACK, "In InfoMsg");

    //create the RequestLine.
    SipRequestLine siprequestline(SIP_INFO, toUrl, getLocalIp());
    setRequestLine(siprequestline);

    cpLog(LOG_DEBUG_STACK, "Completed the requestLine");

    //create the Via header.
    SipVia sipvia("", getLocalIp());


    sipvia.setHost(getLocalIp());
    //The default port number is set automatically.


    sipvia.setPort(fromPort);

    Data lversion("2.0");
    sipvia.setprotoVersion(lversion);  // do we need to get this from some place.
    setVia(sipvia, 0);

    cpLog(LOG_DEBUG_STACK, "Completed the via header");

    //create the From header
    SipFrom sipfrom("", getLocalIp());
    //getUserName, and getDisplayName are extern functions,
    //which contains the user's details. Set from command line?  TODO:
    Data user = theSystem.getUserName();
    sipfrom.setUser(user);
    Data dispName = theSystem.getDisplayName();
    sipfrom.setDisplayName(dispName);
    Data port(fromPort);
    sipfrom.setHost(getLocalIp());
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
    SipCallId callId("", getLocalIp());
    setCallId(callId);

    cpLog(LOG_DEBUG_STACK, "Completed the call id header");

    //create the CSeq header

    SipCSeq cseq(SIP_INFO, Data("1"), getLocalIp());
    setCSeq(cseq);

    //create the subject.
    SipSubject subject(INFO_SUBJECT, getLocalIp());
    setSubject(subject);

}


InfoMsg::InfoMsg( const Data& data, const string& local_ip )
        : SipCommand(local_ip)
{
    SipCommand::decode(data);
}    // InfoMsg::InfoMsg( Data& data )


Method InfoMsg::getType() const
{
    return SIP_INFO;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
