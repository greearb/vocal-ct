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

#include "Data.hxx"
#include "RegisterMsg.hxx"
#include "SipCSeq.hxx"
#include "SipCallId.hxx"
#include "SipContact.hxx"
#include "SipFrom.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequestLine.hxx"
#include "SipRequire.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipTo.hxx"
#include "SipUnsupported.hxx"
#include "SipUrl.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"

using namespace Vocal;


RegisterMsg::RegisterMsg(const string& local_ip)
        : SipCommand(local_ip, "RegisterMsg")
{
    setRegisterDetails();
}


RegisterMsg::RegisterMsg(const RegisterMsg& src)
        : SipCommand(src)
{
}


RegisterMsg::RegisterMsg(const Data& data, const string& local_ip)
        : SipCommand(local_ip, "RegisterMsg")
{
    SipCommand::decode(data);
}


RegisterMsg::RegisterMsg(const RegisterMsg& msg, enum RegisterMsgForward)
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


RegisterMsg&
RegisterMsg::operator=( const RegisterMsg& src )
{
    if ( &src != this )
    {
        SipCommand::operator=( src );

        // Copy data member here
    }

    return ( *this );
}


bool 
RegisterMsg::operator ==(const RegisterMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}


void RegisterMsg::setRegisterDetails()
{
    cpLog(LOG_DEBUG_STACK, "In setRegisterDetails");
    //construct the To header.
    Data toString = theSystem.getUrlToRegister(); //should return sip:....
    Sptr <BaseUrl> toUrl;

    if (toString.length() == 0)
    {
        toString = "sip:";
        toString += getLocalIp();
	toUrl = BaseUrl::decode(toString, getLocalIp());
    }
    else
    {
        toUrl = BaseUrl::decode(toString, getLocalIp());
    }

    cpLog(LOG_DEBUG_STACK, "toString: %s\n",
          toString.c_str());

    SipTo sipto(toUrl, getLocalIp());
    setTo(sipto);

    //construct the From header.
    SipFrom sipfrom("", getLocalIp());
    sipfrom.setTag("candela");
    Data user = theSystem.getUserName();
    sipfrom.setUser(user);
    Data dispName = theSystem.getDisplayName();

    sipfrom.setHost(getLocalIp());
    setFrom(sipfrom);

    //Construct the request line.
    //This is the domain of the registrar. Read from theSystem, or get from the To
    //field.
    Sptr <BaseUrl> requestUrl = new SipUrl("", getLocalIp());
    Data registerDomain = theSystem.getRegisterDomain();
    if (registerDomain.length() == 0)
    {
        //get from the To field.
        registerDomain = sipto.getHost();
    }
    if (requestUrl.getPtr() != 0)
    {
	if (requestUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(requestUrl.getPtr()));
	    sipUrl->setHost(registerDomain);
	}
    }

    SipRequestLine requestLine(SIP_REGISTER, requestUrl, getLocalIp());

    cpLog(LOG_DEBUG_STACK, "requestLine: %s\n", requestLine.encode().c_str());

    setRequestLine(requestLine);

    //set the Via.
    SipVia via("", getLocalIp());

    via.setHost(getLocalIp());


    setVia(via);

    SipCallId callId("", getLocalIp());
    setCallId(callId);

    //create the CSeq header

    Data cseqval = "1";
    SipCSeq cseq(SIP_REGISTER, cseqval, getLocalIp());
    SipMsg::setCSeq(cseq);

    //add default Expires
    SipExpires sipexpires("", getLocalIp());
    sipexpires.setDelta(Data("3600"));
    SipMsg::setExpires(sipexpires);

    //SipContact contact() TODO after SipContact is fully implemented.
}


Method 
RegisterMsg::getType() const
{
    return SIP_REGISTER;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
