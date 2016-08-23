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

#include "CancelMsg.hxx"
#include "Data.hxx"
#include "SipCallId.hxx"
#include "SipRequire.hxx"
#include "SipRoute.hxx"
#include "SipVia.hxx"

using namespace Vocal;

CancelMsg::CancelMsg(const SipCommand& src)
    : SipCommand(src)
{
    myRequestLine = src.getRequestLine();
    myRequestLine.setMethod(CANCEL_METHOD);

    copyHeader(src, SIP_TO_HDR);
    copyHeader(src, SIP_FROM_HDR);
    copyHeader(src, SIP_CALLID_HDR);
    copyHeader(src, SIP_PROXY_AUTHORIZATION_HDR);
    copyHeader(src, SIP_ROUTE_HDR);
    
    SipCSeq cseq = src.getCSeq();
    cseq.setMethod(CANCEL_METHOD);
    setCSeq(cseq);

    viaPushFront(src.viaFront());
}


CancelMsg& 
CancelMsg::operator =(const CancelMsg& src)
{
    if ( &src != this)
    {
        *static_cast < SipCommand* > (this) = src;
    }
    return (*this);
}


CancelMsg::CancelMsg(const string& local_ip) : SipCommand(local_ip, "CancelMsg")
{
    myRequestLine.setMethod(CANCEL_METHOD);
    SipCSeq cseq( SIP_CANCEL, 0, local_ip);
    setCSeq( cseq );
}


CancelMsg::CancelMsg(const CancelMsg& src)
        : SipCommand(src)
{
}

CancelMsg::CancelMsg(const Data& data, const string& local_ip)
    : SipCommand(local_ip, "CancelMsg")
{
    SipCommand::decode(data);

}


CancelMsg::CancelMsg(const CancelMsg& msg, enum CancelMsgForward)
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


bool 
CancelMsg::operator ==(const CancelMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}


/*** The SipCommand must be the Request which we are cancelling */
void 
CancelMsg::setCancelDetails(const SipCommand& sipCommand)
{
    //copy RequestLine ,CallId, From, To,  fields from sipCommand.
    SipRequestLine srcrequestLine(getLocalIp());
    srcrequestLine = sipCommand.getRequestLine();
    Sptr <BaseUrl> url = srcrequestLine.getUrl();

    SipRequestLine requestLine(SIP_CANCEL, url, getLocalIp());

    setRequestLine(requestLine);

    setCallId(sipCommand.getCallId());
    setFrom(sipCommand.getFrom());
    setTo(sipCommand.getTo());

    SipCSeq cseq = sipCommand.getCSeq();
    //change the method to CANCEL.
    Data method = CANCEL_METHOD;

    setProxyAuthorization(sipCommand.getProxyAuthorization());

    cseq.setMethod(method);
    setCSeq(cseq);

    //copy via details from invite msg.
    copyViaList(sipCommand);

    setContentLength(ZERO_CONTENT_LENGTH);
}


Method 
CancelMsg::getType() const
{
    return SIP_CANCEL;
}


/* Local Variables: */
/* c-file-style:"stroustrup" */
/* c-basic-offset:4 */
/* c-file-offsets:((inclass . ++)) */
/* indent-tabs-mode:nil */
/* End: */
