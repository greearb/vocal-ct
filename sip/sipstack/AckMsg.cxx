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

static const char* const AckMsg_cxx_Version =
    "$Id: AckMsg.cxx,v 1.2 2004/05/04 07:31:14 greear Exp $";

#include "global.h"
#include <cstdlib>
#include <cstdio>

#include "AckMsg.hxx"
#include "SipCommand.hxx"
#include "SipContact.hxx"
#include "SipMsg.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipProxyRequire.hxx"
#include "SipRequire.hxx"
#include "SipRoute.hxx"
#include "SipRecordRoute.hxx"
#include "SipUnsupported.hxx"
#include "SipVia.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "StatusMsg.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;


AckMsg::AckMsg(const AckMsg& src)
        : SipCommand(src)
{
}


AckMsg::AckMsg(const Data& data, const string& local_ip)
        : SipCommand(local_ip, "AckMsg")
{
    SipCommand::decode(data);
}


AckMsg::AckMsg(const string& local_ip)
        : SipCommand(local_ip, "AckMsg")
{
    myRequestLine.setMethod(ACK_METHOD);
    SipCSeq cseq( SIP_ACK, Data((int)0), local_ip);
    setCSeq( cseq );
}


AckMsg::AckMsg(const SipCommand& src, const SipVia& via, const SipCSeq& cseq)
        : SipCommand(src, via, cseq)
{
    myRequestLine.setMethod(ACK_METHOD);
}


AckMsg::AckMsg(StatusMsg& statusMsg, const string& local_ip)
        : SipCommand(statusMsg, local_ip, "AckMsg")
{
    myRequestLine.setMethod(ACK_METHOD);
    if (!statusMsg.viaEmpty())
    {
        viaPushFront(statusMsg.viaFront());
    }
    
    Data data(statusMsg.getCSeq().getCSeqData());
    SipCSeq cseq(SIP_ACK, data, local_ip);
    setCSeq(cseq);

    // If there is a route, then remove the first one and make that the request line URI
    if ( getNumRoute() > 0 )
    {
        Sptr<BaseUrl> dest = getRoute(0).getUrl();
        myRequestLine.setUrl(dest);
        removeRoute(0);
    }

    // If there is a contact header in the status msg, then make it a route header in the ACK
    if(statusMsg.getNumContact() >= 1)
    {
        SipRoute route("", getLocalIp());
        route.setUrl(statusMsg.getContact(0).getUrl());
        setRoute(route, -1);
    }

cpLog(LOG_DEBUG, "ACK constructed:\n%s", this->encode().logData());

}


AckMsg::~AckMsg()
{
}


AckMsg& 
AckMsg::operator =(const AckMsg& src)
{
    if ( &src != this)
    {
	SipCommand::operator=( src );
    }
    return (*this);
}


bool 
AckMsg::operator ==(const AckMsg& src) 
{
   
   return (  *static_cast < SipCommand* > (this) == src);
   
}


AckMsg::AckMsg(const AckMsg& msg, enum AckMsgForward)
    : SipCommand(msg)
{
    Sptr<BaseUrl> dest = getTo().getUrl();

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

    myRequestLine.setUrl(dest);
}

// to be deleted
void
AckMsg::setAckDetails(const StatusMsg& statusMsg)
{

    //only the CSeq will change.

    //the Via is set to the last one.(which would contain the
    //originating client details
    assert( statusMsg.getNumVia() > 0 );


    // get the top most via, since this holds good for both 200 and
    // other > 200 final responses.

    const SipVia& via = statusMsg.getVia(0);
    setVia(via);

    // construct the To field
    const SipTo& to = statusMsg.getTo();
    setTo(to);

    Sptr<BaseUrl> url = to.getUrl();

    //construct the request line.
    SipRequestLine siprequestline(SIP_ACK, url, getLocalIp());
    setRequestLine(siprequestline);

    //construct the From field
    setFrom(statusMsg.getFrom());

    //construct the CallId field
    setCallId(statusMsg.getCallId());

    //construct the CSeq field
    const SipCSeq& cseq = statusMsg.getCSeq();

    Data strval(cseq.getCSeqData());

    SipCSeq newcseq(SIP_ACK, strval, getLocalIp());

    setCSeq(newcseq);

    // construct the route list, based on the record route.  For
    // status code 400 and more ignore the Record route list

    int numRecordRoute = 0;
    if(statusMsg.getStatusLine().getStatusCode()  == 200)
    {
        numRecordRoute = statusMsg.getNumRecordRoute();
    }
    SipRoute route("", getLocalIp());

    if (numRecordRoute)
    {
        for (int i = numRecordRoute - 1; i >= 0 ; i--)
        {
            const SipRecordRoute& recordroute = statusMsg.getRecordRoute(i);
            route.setUrl(recordroute.getUrl());
            setRoute(route);  // to beginning
        }

    }

    // if the status msg contains a contact header, then, copy that to
    // the route. There can be only one contact field in the response
    // (200) the contact field is left alone.
    int numContact = statusMsg.getNumContact();
    cpLog( LOG_DEBUG_STACK, "add %d contacts to route", numContact );
    if (numContact > 0)
    {
        // contact present.  get the last contact. This is the one
        // inserted by the originating client.
        const SipContact& contact = statusMsg.getContact(numContact - 1);
        route.setUrl(contact.getUrl());
        setRoute(route);  // to beginning
        setNumContact(0);
    }

    // figure out the URI
    Sptr<BaseUrl> dest;
    dest = getTo().getUrl();

    assert( getNumContact() == 0);

    int numRoute = getNumRoute();
    //For bis-9 compliance this should be changed and should check if the first route is 
    //a loose route, if so 
    //the request-line should be the contact instead
    if (numRoute != 0)
    {
        dest = getRoute(0).getUrl();
        // remove that route.
        removeRoute(0);
    }

    myRequestLine.setUrl( dest);
    setContentLength(ZERO_CONTENT_LENGTH);
}


Method AckMsg::getType() const
{
    return SIP_ACK;
}

/* Local Variables: */
/* c-file-style:"stroustrup" */
/* c-basic-offset:4 */
/* c-file-offsets:((inclass . ++)) */
/* indent-tabs-mode:nil */
/* End: */
