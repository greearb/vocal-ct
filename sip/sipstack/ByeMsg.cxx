
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
#include "ByeMsg.hxx"
#include "SipAuthorization.hxx"
#include "SipContact.hxx"
#include "SipContentDisposition.hxx"
#include "SipEncryption.hxx"
#include "SipFrom.hxx"
#include "SipHide.hxx"
#include "SipMaxForwards.hxx"
#include "SipProxyAuthorization.hxx"
#include "SipProxyRequire.hxx"
#include "SipRequire.hxx"
#include "SipRecordRoute.hxx"
#include "SipRoute.hxx"
#include "SipVia.hxx"
#include "StatusMsg.hxx"

using namespace Vocal;


//ByeMsg::ByeMsg(const SipCommand& ackCommand)

/* If this is the caller(issued INVITE, then the Bye Msg should be
sent to the Contact obtained in the 200 Status msg.  If this is the
callee, then the Bye Msg should be sent to the Contact obtained in the
INVITE msg , or the ACK msg.  It is up to the User Agent to send the
corrent SipMsg from which the Contact will be taken */

ByeMsg::ByeMsg(const string& local_ip) : SipCommand(local_ip, "ByeMsg")
{
    myRequestLine.setMethod(BYE_METHOD);
    SipCSeq cseq( SIP_BYE, "0", local_ip );
    setCSeq( cseq );
}


ByeMsg::ByeMsg(const ByeMsg& src) : SipCommand(src)
{
}

ByeMsg::ByeMsg(const SipCommand& src)
        : SipCommand(src)
{
    myRequestLine.setMethod(BYE_METHOD);
    SipCSeq cseq = src.getCSeq();
    int value = cseq.getCSeqData().convertInt();
    value++;
    SipCSeq newcseq(SIP_BYE, Data(value), getLocalIp());
    setCSeq(newcseq);
   // Flip To & From when using ACK to generate BYE
    const SipFrom& srcfrom = src.getFrom();
    const SipTo& srcto = src.getTo();

    //set the To header.

    SipTo to(srcfrom.getUrl(), getLocalIp());
    to.setDisplayName( srcfrom.getDisplayName() );
    to.setTag( srcfrom.getTag() );
    to.setToken( srcfrom.getToken() );
    to.setQstring( srcfrom.getQstring() );
    setTo(to);

    //set the From header.
    SipFrom from(srcto.getUrl(), getLocalIp());
    from.setDisplayName( srcto.getDisplayName() );
    from.setTag( srcto.getTag() );
    from.setToken( srcto.getToken() );
    from.setQstring( srcto.getQstring() );
    setFrom(from);
}


ByeMsg::ByeMsg(const SipCommand& src, const SipVia& via, const SipCSeq& cseq)
        : SipCommand(src, via, cseq)
{
    myRequestLine.setMethod(BYE_METHOD);
}


ByeMsg::ByeMsg(const StatusMsg& statusMsg, const string& local_ip)
        : SipCommand(statusMsg, local_ip, "ByeMsg")
{
    myRequestLine.setMethod(BYE_METHOD);
    cpLog(LOG_ERR, "myRequestLine: %s, local_ip: %s\n",
          myRequestLine.encode().c_str(), local_ip.c_str());

    SipCSeq cseq = statusMsg.getCSeq();
    int value = cseq.getCSeqData().convertInt();

    value++;

    SipCSeq newcseq(SIP_BYE, Data(value), local_ip);
    setCSeq(newcseq);

    // If there is a route, then remove the first one and make that the request line URI
    if ( getNumRoute() > 0 )
    {
        Sptr<BaseUrl> dest = getRoute(0).getUrl();
        myRequestLine.setUrl(dest);
        removeRoute(0);
        cpLog(LOG_ERR, "myRequestLine2: %s, dest: %s\n",
              myRequestLine.encode().c_str(), dest->encode().c_str());
    }

    // If there is a contact header in the status msg, then make it a route header in the BYE
    if(statusMsg.getNumContact() >= 1)
    {
        SipRoute route("", getLocalIp());
        route.setUrl(statusMsg.getContact(0).getUrl());
        setRoute(route, -1);
    }

    cpLog(LOG_ERR, "BYE constructed:\n%s", this->encode().logData());

}


ByeMsg::ByeMsg(const ByeMsg& msg, enum ByeMsgForward) :
    SipCommand(msg)
{
    *this = msg;

    Sptr<BaseUrl> dest = getTo().getUrl();

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


ByeMsg& ByeMsg::operator=(const ByeMsg& src)
{
    if ( &src != this)
    {
        *static_cast < SipCommand* > (this) = src;
    }
    return (*this);
}

bool ByeMsg::operator ==(const ByeMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}

ByeMsg::ByeMsg(const Data& data, const string& local_ip)
        : SipCommand(local_ip, "ByeMsg")
{
        SipCommand::decode(data);  
}


void ByeMsg::setByeDetails( const SipMsg& sipMessage)
{
    // TODO:  what should listenPort mean???
    //We will use the default port:5060.

    /*** swap from and to only if callee(got INVITE or ACK) . If
     * caller, then don't swap */
    //check the Url for the Method.

    if ( dynamic_cast < const SipCommand* > (&sipMessage) )
    {
        const SipFrom& srcfrom = sipMessage.getFrom();
        const SipTo& srcto = sipMessage.getTo();
        //set the To header.
        SipTo to(srcfrom.getUrl(), getLocalIp());
        to.setDisplayName( srcfrom.getDisplayName() );
        to.setTag( srcfrom.getTag() );
        to.setToken( srcfrom.getToken() );
        to.setQstring( srcfrom.getQstring() );
        setTo(to);

        //set the From header.
        SipFrom from(srcto.getUrl(), getLocalIp());
        from.setDisplayName( srcto.getDisplayName() );
        from.setTag( srcto.getTag() );
        from.setToken( srcto.getToken() );
        from.setQstring( srcto.getQstring() );
        setFrom(from);

    }
    else if ( dynamic_cast < const StatusMsg* > (&sipMessage) )
    {
        setTo(sipMessage.getTo());
        setFrom(sipMessage.getFrom());
    }

    Sptr<BaseUrl> url = getTo().getUrl();

    SipVia sipvia("", getLocalIp());
    //create the via header. This contains details present in the toUrl.
    Sptr <BaseUrl> toUrl = getFrom().getUrl();

    //construct the request line.
    SipRequestLine siprequestline(SIP_BYE, url, getLocalIp());
    setRequestLine(siprequestline);

    //assign the CallId.
    setCallId(sipMessage.getCallId());

    //create the CSeq header
    SipCSeq cseq = sipMessage.getCSeq();
    int value = cseq.getCSeqData().convertInt();
    //increment cseq.
    value++;

    SipCSeq newcseq(SIP_BYE, Data(value), getLocalIp());
    setCSeq(newcseq);
    if ( dynamic_cast < const StatusMsg* > (&sipMessage) )
    {
        //construct the route list, based on the record route.
        int numRecordRoute = sipMessage.getNumRecordRoute();
        if (numRecordRoute)
        {
            for (int i = numRecordRoute - 1; i >= 0; i--)
                // for(int i = 0; i < numRecordRoute; i++)
            {
                const SipRecordRoute& recordroute = sipMessage.getRecordRoute(i);

                SipRoute route("", getLocalIp());
                route.setUrl(recordroute.getUrl());
                setRoute(route);
            }
        }

        if (sipMessage.getNumContact() == 1)
        {
            SipRoute route("", getLocalIp());
            route.setUrl(sipMessage.getContact().getUrl());
            setRoute(route);

            //remove the contact header.
            setNumContact(0);
        }
    }
    else
    {
        if ( dynamic_cast < const SipCommand* > (&sipMessage) )
        {
            //construct the route list, based on the record route.
            int numRecordRoute = sipMessage.getNumRecordRoute();
            if (numRecordRoute)
            {

                //for(int i = numRecordRoute-1; i >= 0; i--)
                for (int i = 0 ; i < numRecordRoute; i++)
                {
                    const SipRecordRoute& recordroute = sipMessage.getRecordRoute(i);
                    SipRoute route("", getLocalIp());
                    route.setUrl(recordroute.getUrl());
                    setRoute(route);
                }
            }

            if (sipMessage.getNumContact() == 1)
            {
                SipRoute route("", getLocalIp());
                route.setUrl(sipMessage.getContact().getUrl());
                setRoute(route);

                //remove the contact header.
                setNumContact(0);
            }
        }
        else
        {
            assert(0);
        }
    }

    // set up the URI
    Sptr <BaseUrl> dest;
    dest = getTo().getUrl();

    assert( getNumContact() == 0);

    int numRoute = getNumRoute();
    if (numRoute != 0 )
    {
        dest = getRoute(0).getUrl();
        //remove that route.
        removeRoute(0);
    }
    else
    {
        dest = getTo().getUrl();
    }

    setContentLength(ZERO_CONTENT_LENGTH);
    myRequestLine.setUrl(dest);

    //Set the transport of Request line.
    sipvia.setprotoVersion("2.0");
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl = (SipUrl*)(toUrl.getPtr());
    
	    sipvia.setHost(sipUrl->getHost());
	    sipvia.setPort(sipUrl->getPort().convertInt());
            if(myRequestLine.getTransportParam().length())
            {
                if(myRequestLine.getTransportParam() == "tcp")
                {
                    sipvia.setTransport("TCP");
                }
            }
	}
    }
    //add via to the top
    setVia(sipvia, 0);
}


Method ByeMsg::getType() const
{
    return SIP_BYE;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
