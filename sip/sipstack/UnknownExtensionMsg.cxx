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

#include "global.h"
#include <cstdlib>
#include <cstdio>
#include "Data.hxx"
#include "cpLog.h"
#include "symbols.hxx"
#include "SystemInfo.hxx"

#include "UnknownExtensionMsg.hxx"

#include "SipVia.hxx"
#include "SipContact.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipSdp.hxx"

using Vocal::SDP::SdpMedia;
using Vocal::SDP::SdpConnection;
using Vocal::SDP::SdpSession;

using namespace Vocal;

UnknownExtensionMsg::UnknownExtensionMsg(const string& local_ip)
        : SipCommand(local_ip, "UnknownExtensionMsg")
{
}


UnknownExtensionMsg::UnknownExtensionMsg(const UnknownExtensionMsg& src)
        : SipCommand(src)
{
}


UnknownExtensionMsg&
UnknownExtensionMsg::operator=( const UnknownExtensionMsg& src )
{
    if ( &src != this )
    {
        SipCommand::operator=( src );

        // Copy data member here
    }

    return ( *this );
}


bool 
UnknownExtensionMsg::operator ==(const UnknownExtensionMsg& src) 
{
   return (  *static_cast < SipCommand* > (this) == src);
}


UnknownExtensionMsg::UnknownExtensionMsg(Sptr<BaseUrl> toUrl, const string& local_ip,
                                         int listenPort, int rtpPort)
        : SipCommand(local_ip, "UnknownExtensionMsg")
{
    cpLog(LOG_DEBUG_STACK, "C'tor of UnknownExtensionMsg");

    setUnknownExtensionMsgDetails(toUrl, listenPort, rtpPort);
}


UnknownExtensionMsg::UnknownExtensionMsg(const UnknownExtensionMsg& msg, enum UnknownExtensionMsgForward)
    : SipCommand(msg.getLocalIp(), "UnknownExtensionMsg")
{
    *this = msg;

    Data test1 = getRequestLine().getMethod();

    Data test2 = getCSeq().getMethod();
    if (test1 == test2)
    {
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
	if (dest.getPtr() != 0)
	{
	    SipRequestLine reqLine = getRequestLine();
	    reqLine.setUrl( dest );
	    setRequestLine( reqLine );
	}
    }
    else
    {
        //EXCEPTION
    }

}


void UnknownExtensionMsg::setUnknownExtensionMsgDetails(Sptr <BaseUrl> toUrl,
                                                        int listenPort, int rtpPort)
{
    //build and store in sipMessage.
    //build the non-SDP part here, and call SdpMsg class to build the SDP part.

    //build each header , and store it in that header class.
    cpLog(LOG_DEBUG_STACK, "In UnknownExtensionMsg");

    //create the RequestLine.
    SipRequestLine siprequestline(getLocalIp());
    Data method1 = siprequestline.getMethod();
    SipRequestLine siprequestline1(method1, toUrl, getLocalIp());

    setRequestLine(siprequestline);
    SipCSeq cseq1("", getLocalIp());
    Data cseqval = "1";
    Data method2 = cseq1.getMethod();
   
    setCSeq(SipCSeq(method2, cseqval, getLocalIp()));

    cpLog(LOG_DEBUG_STACK, "Completed the requestLine");

    //create the Via header.
    SipVia sipvia("", getLocalIp());
    //copy the details of the current host, and port into the Via.

    sipvia.setHost(local_ip);
    //The default port number is set automatically.


    Data newport(listenPort);
    if (newport.length() > 0)
    {
        sipvia.setPort(newport.convertInt());
    }
   
    sipvia.setprotoVersion(Data("2.0"));  // do we need to get this from some place.
    setVia(sipvia, 0);

    cpLog(LOG_DEBUG_STACK, "Completed the via header");

    //create the From header
    SipFrom unknownMsgFrom("", getLocalIp());
    //getUserName, and getDisplayName are extern functions,
    //which contains the user's details. Set from command line?
    Data user = theSystem.getUserName();
    unknownMsgFrom.setUser(user);
    Data dispName = theSystem.getDisplayName();
    unknownMsgFrom.setDisplayName(dispName);
    Data port(listenPort);
    unknownMsgFrom.setHost( Data(local_ip));
    unknownMsgFrom.setPort(port);
    setFrom(unknownMsgFrom);

    cpLog(LOG_DEBUG_STACK, "Completed the from header");


    //create the To header
    Sptr <BaseUrl> requestUrl = siprequestline.getUrl();

   
    if (toUrl.getPtr() != 0)
    {
	SipTo unknownMsgTo(requestUrl, getLocalIp());
	Data name = toUrl->getUserValue();
	unknownMsgTo.setDisplayName(name);
	setTo(unknownMsgTo);
    }

    cpLog(LOG_DEBUG_STACK, "Completed the to header");

    //create the callId header
    setCallId(SipCallId("", getLocalIp()));

    cpLog(LOG_DEBUG_STACK, "Completed the call id header");

    //create the sdp.
    SipSdp sdp(getLocalIp());
    //Change this so that we call functions in SDP, and SDP abstracts this
    //from us.
    // set connection details.
    SdpSession sdpDesc;

    Data connAddr = local_ip;
    /*char connAddrstr[256];
    strcpy(connAddrstr, connAddr); */
    SdpConnection connection;
    LocalScopeAllocator lo;
    connection.setUnicast(connAddr.getData(lo));
    sdpDesc.setConnection(connection, __FUNCTION__);

    SdpMedia* media = new SdpMedia;
    media->setPort(rtpPort);

    sdpDesc.addMedia(media);
    //Change this to use NTP time
    srandom((unsigned int)time(0));
    int verStr = random();


    //Change this to use  NTP time
    srandom((unsigned int)time(0));
    int sessionStr = random();

    sdpDesc.setVersion(verStr);
    sdpDesc.setSessionId(sessionStr);

    sdp.setSdpDescriptor(sdpDesc);
    sdp.setUserName(theSystem.getUserName());

    sdp.setSessionName(theSystem.getSessionName());
    sdp.setAddress(local_ip);
    setContentData(&sdp, 0);
}


UnknownExtensionMsg::UnknownExtensionMsg( Data& data, const string& local_ip )
        : SipCommand(local_ip, "UnknownExtensionMsg")
{
    SipCommand::decode(data);
}


Method 
UnknownExtensionMsg::getType() const
{
    return SIP_UNKNOWN;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
