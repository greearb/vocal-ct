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

static const char* const OptionsMsg_cxx_Version =
    "$Id: OptionsMsg.cxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include "Data.hxx"
#include "OptionsMsg.hxx"
#include "SipContact.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRoute.hxx"
#include "SipSdp.hxx"
#include "SipServer.hxx"
#include "SipVia.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"

using namespace Vocal;
using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpConnection;
using Vocal::SDP::SdpMedia;


OptionsMsg::OptionsMsg(const string& local_ip)
        : SipCommand(local_ip, "OptionsMsg")
{
    myRequestLine.setMethod(OPTIONS_METHOD);
    SipCSeq cseq( SIP_OPTIONS, 0, local_ip );
    setCSeq( cseq );
}


OptionsMsg::OptionsMsg(const OptionsMsg& src)
        : SipCommand(src)
{
}


OptionsMsg&
OptionsMsg::operator=( const OptionsMsg& src )
{
    if ( &src != this )
    {
        SipCommand::operator=( src );

        // Copy data member here
    }

    return ( *this );
}


bool 
OptionsMsg::operator ==(const OptionsMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}


OptionsMsg::OptionsMsg(Sptr <BaseUrl> toUrl, const string& local_ip,
                       int listenPort, int rtpPort)
        : SipCommand(local_ip, "OptionsMsg")
{
    cpLog(LOG_DEBUG_STACK, "C'tor of OptionsMsg");

    setOptionsDetails(toUrl, listenPort, rtpPort);
}


OptionsMsg::OptionsMsg(const OptionsMsg& msg, enum OptionsMsgForward)
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


void 
OptionsMsg::setOptionsDetails(Sptr <BaseUrl> toUrl,
                              int listenPort, int rtpPort)
{
    //build and store in sipMessage.
    //build the non-SDP part here, and call SdpMsg class to build the SDP part.

    //build each header , and store it in that header class.
    cpLog(LOG_DEBUG_STACK, "In OptionsMsg");

    //create the RequestLine.
    SipRequestLine siprequestline(SIP_OPTIONS, toUrl, getLocalIp());
    setRequestLine(siprequestline);

    cpLog(LOG_DEBUG_STACK, "Completed the requestLine");

    //create the Via header.
    SipVia sipvia("", getLocalIp());
    //copy the details of the current host, and port into the Via.

    sipvia.setHost(getLocalIp());
    //The default port number is set automatically.


    Data newport(listenPort);
    if (newport.length() > 0)
    {
        sipvia.setPort(newport.convertInt());
    }

    Data lversion("2.0");
    sipvia.setprotoVersion(lversion);  // do we need to get this from some place.
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
    sipfrom.setHost( Data(getLocalIp()));
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

    SipCSeq cseq(SIP_OPTIONS, Data("1"), getLocalIp());
    SipMsg::setCSeq(cseq);

    //create the sdp.
    SipSdp sdp(getLocalIp());
    //Change this so that we call functions in SDP, and SDP abstracts this
    //from us.
    // set connection details.
    SdpSession sdpDesc;

    Data connAddr = getLocalIp();
    SdpConnection connection;
    LocalScopeAllocator lo;
    connection.setUnicast(connAddr.getData(lo));
    sdpDesc.setConnection(connection);

    SdpMedia* media = new SdpMedia;
    media->setPort(rtpPort);

    sdpDesc.addMedia(media);
    //If possible use NTP time.
    srandom((unsigned int)time(NULL));
    int verStr = random();


    //If possible , use NTP time.
    srandom((unsigned int)time(NULL));
    int sessionStr = random();

    sdpDesc.setVersion(verStr);
    sdpDesc.setSessionId(sessionStr);

    sdp.setSdpDescriptor(sdpDesc);
    sdp.setUserName(theSystem.getUserName());

    sdp.setSessionName(theSystem.getSessionName());
    sdp.setAddress(getLocalIp());
    setContentData(&sdp, 0);
}


OptionsMsg::OptionsMsg( const Data& data, const string& local_ip )
        : SipCommand(local_ip, "OptionsMsg")
{

    SipCommand::decode(data);

}


Method 
OptionsMsg::getType() const
{
    return SIP_OPTIONS;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
