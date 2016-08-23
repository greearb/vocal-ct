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

#include "SipCallLeg.hxx"
#include "SipExpires.hxx"
#include "SipMsg.hxx"
#include "SipRoute.hxx"
#include "SipVia.hxx"
#include "SubscribeMsg.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"
#include "symbols.hxx"

using namespace Vocal;


SubscribeMsg::SubscribeMsg(const string& local_ip)
        : SipCommand(local_ip, "SubscribeMsg")
{
    myRequestLine.setMethod(SUBSCRIBE_METHOD);
    SipCSeq cseq( SIP_SUBSCRIBE, Data(0), getLocalIp() );
    setCSeq( cseq );

    //set to and from of host creating SUBSCRIBE msg
    SipFrom from("", getLocalIp());
    Data user = theSystem.getUserName();
    from.setUser(user);
    Data dispName = theSystem.getDisplayName();
    from.setDisplayName(dispName);
    from.setHost( Data(local_ip));
    from.setPort(Data(5060));  //port defaults to 5060
    setFrom(from);

    SipTo to( from.getUrl(), getLocalIp());
    to.setDisplayName( from.getDisplayName());
    setTo(to);

    //use the register url from the system as the request url, if it exists.  If it
    //doesn't exist, use the to url as the request url.
    Sptr <BaseUrl> requestUrl;
    Data registerDomain = theSystem.getRegisterDomain();
    if (registerDomain.length() == 0)
    {
        //get from the To field.
        registerDomain = to.getHost();
    }
    if (requestUrl.getPtr() != 0)
    {
	if (requestUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(requestUrl.getPtr()));	    
	    sipUrl->setHost(registerDomain);
	}
    }
    setSubscribeDetails(from.getUrl(), requestUrl);
}

    
SubscribeMsg::SubscribeMsg(const Data& data, const string& local_ip)
        : SipCommand(local_ip, "SubscribeMsg")
{
    try
    {
        SipCommand::decode(data);
    }
    catch (SipParserException& e)
    {
        cpLog(LOG_ERR, "Failed to parse the SIP msg, since: %s", e.getDescription().c_str());
    }
}


//call-member subscriptions
SubscribeMsg::SubscribeMsg(const SipCallLeg& callLeg, Sptr <BaseUrl> reqUrl)
        : SipCommand(callLeg.getLocalIp(), "SubscribeMsg")
{
    //set call-id, from, and to
    setCallId(callLeg.getCallId());
    setTo(callLeg.getTo());
    setFrom(callLeg.getFrom());

    //set the other headers of SUBSCRIBE message
    Sptr <BaseUrl> fromUrl = callLeg.getFrom().getUrl();
    setSubscribeDetails(fromUrl, duplicateUrl(reqUrl));
}


//third-party subscriptions
SubscribeMsg::SubscribeMsg(const SipCallId& callId, const SipTo& to,
                           const string& local_ip, int port)
        : SipCommand(local_ip, "SubscribeMsg")
{
    //set the call-id and to
    setCallId(callId);
    setTo(to);

    //set from
    SipFrom from("", getLocalIp());
    Data user = theSystem.getUserName();
    from.setUser(user);
    Data dispName = theSystem.getDisplayName();
    from.setDisplayName(dispName);
    Data listenPort(port);
    from.setHost( Data(local_ip));
    from.setPort(listenPort);
    setFrom(from);

    //set the other headers of SUBSCRIBE message
    Sptr <BaseUrl> toUrl = to.getUrl();
    Sptr <BaseUrl> fromUrl = from.getUrl();
    setSubscribeDetails(fromUrl, toUrl);
}

    
//copy constructor
SubscribeMsg::SubscribeMsg(const SubscribeMsg& src)
        : SipCommand(src)
{}


SubscribeMsg::SubscribeMsg( const SubscribeMsg& msg, enum SubscribeMsgForward )
    : SipCommand(msg)
{
    *this = msg;

    Sptr <BaseUrl> dest = getTo().getUrl();

    if ( getNumRoute() )
    {
        int numRoute = getNumRoute();
        if (numRoute != 0 )
        {
            dest = getRoute(0).getUrl();
            //remove that route.
            removeRoute(0);
        }
    }

    SipRequestLine reqLine = getRequestLine();
    reqLine.setUrl( dest );
    setRequestLine( reqLine );
}

SubscribeMsg& SubscribeMsg::operator=(const SubscribeMsg& src)
{
    if (&src != this)
    {
        SipCommand::operator=(src);
    }
    return (*this);
}

    
bool SubscribeMsg::operator ==(const SubscribeMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}


Method 
SubscribeMsg::getType() const
{
    return SIP_SUBSCRIBE;
}

void 
SubscribeMsg::setSubscribeDetails(Sptr <BaseUrl> fromUrl, 
                                       Sptr <BaseUrl> toUrl)
{
    //set the CSeq
    SipCSeq cseq(SIP_SUBSCRIBE, Data(1), getLocalIp());
    setCSeq(cseq);

    //set the Via
    SipVia sipVia("", getLocalIp());
    sipVia.setprotoVersion("2.0");
    if (fromUrl.getPtr() != 0)
    {
	if (fromUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(fromUrl.getPtr()));
	    sipVia.setHost(sipUrl->getHost());
	    sipVia.setPort(sipUrl->getPort().convertInt());
	}
    }
    setVia(sipVia);

    //add default Expires
    SipExpires sipexpires("", getLocalIp());
    sipexpires.setDelta(Data("3600"));
    setExpires(sipexpires);

    //set the request line
    SipRequestLine requestLine(SIP_SUBSCRIBE, toUrl, getLocalIp());
    setRequestLine(requestLine);
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
