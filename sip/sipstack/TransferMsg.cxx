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

static const char* const TransferMsg_cxx_Version =
    "$Id: TransferMsg.cxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include "global.h"
#include <cstdlib>
#include <cstdio>
#include "Data.hxx"
#include "cpLog.h"

#include "symbols.hxx"
#include "SystemInfo.hxx"

#include "TransferMsg.hxx"
#include "SipVia.hxx"
#include "SipContact.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipRequire.hxx"
#include "SipRetryAfter.hxx"
#include "SipRecordRoute.hxx"
#include "SipRoute.hxx"
#include "SipServer.hxx"
#include "SipUnsupported.hxx"
#include "SipWarning.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SipTransferTo.hxx"

using namespace Vocal;


TransferMsg::TransferMsg(const string& local_ip)
        : SipCommand(local_ip, "TransferMsg")
{}



TransferMsg::TransferMsg(const TransferMsg& src)
        : SipCommand(src)
{
}


TransferMsg& TransferMsg::operator =(const TransferMsg& src)
{
    if ( &src != this)
    {
        //copy details contained in the base class.
        *static_cast < SipCommand* > (this) = src;
    }
    return (*this);
}

TransferMsg::TransferMsg(const StatusMsg& statusMsg, Sptr <BaseUrl> transferToUrl,
                         const string& local_ip)
        : SipCommand(local_ip, "TransferMsg")
{
    cpLog(LOG_DEBUG_STACK, "C'tor of Transfer");

    if (transferToUrl.getPtr() != 0)
    {
	cpLog(LOG_DEBUG_STACK, "SipUrl: %s", transferToUrl->encode().logData());
    }
    int type = statusMsg.getType();

    setTransferDetails(statusMsg, type, transferToUrl);
}

TransferMsg::TransferMsg(const SipCommand& sipCommand, Sptr <BaseUrl> transferToUrl)
    : SipCommand(sipCommand.getLocalIp(), "TransferMsg")
{
    cpLog(LOG_DEBUG_STACK, "C'tor of Transfer");

    if (transferToUrl.getPtr() != 0)
    {
	cpLog(LOG_DEBUG_STACK, "SipUrl: %s", transferToUrl->encode().logData());
    }
    int type = sipCommand.getType();

    setTransferDetails(sipCommand, type, transferToUrl);
}

TransferMsg::TransferMsg(const TransferMsg& msg, enum TransferMsgForward)
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

void TransferMsg::setTransferDetails(const SipMsg& sipMsg,
                                     int type,
                                     Sptr <BaseUrl> transferToUrl)
{
    if (transferToUrl.getPtr() != 0)
    {
	cpLog(LOG_DEBUG_STACK, "SipUrl:setTransferDetails: %s",
	      transferToUrl->encode().logData());
    }



    if (type == SIP_INVITE)
    {
        SipFrom srcfrom = sipMsg.getFrom();
        SipTo srcto = sipMsg.getTo();
        //set the To header.
        SipTo to(srcfrom.getUrl(), getLocalIp());
        setTo(to);

        //set the From header.
        SipTo srcTo = sipMsg.getTo();
        SipFrom from(srcTo.getUrl(), getLocalIp());
        setFrom(from);

        //construct the route list, based on the record route.
        int numRecordRoute = sipMsg.getNumRecordRoute();
        if (numRecordRoute)
        {
            for (int i = 0 ; i < numRecordRoute; i++)
            {
                SipRecordRoute recordroute = sipMsg.getRecordRoute(i);

                SipRoute route("", getLocalIp());
                route.setUrl(recordroute.getUrl());
                setRoute(route);
            }
        }

        if (sipMsg.getNumContact() == 1)
        {
            SipRoute route("", getLocalIp());
            route.setUrl(sipMsg.getContact().getUrl());
            setRoute(route);

            //remove the contact header.
            setNumContact(0);
        }
    }

    else if (type == SIP_STATUS)
    {
        setTo(sipMsg.getTo());
        setFrom(sipMsg.getFrom());

        //construct the route list, based on the record route.
        int numRecordRoute = sipMsg.getNumRecordRoute();
        if (numRecordRoute)
        {
            for (int i = numRecordRoute - 1; i >= 0; i--)
            {
                SipRecordRoute recordroute = sipMsg.getRecordRoute(i);

                SipRoute route("", getLocalIp());
                route.setUrl(recordroute.getUrl());
                setRoute(route);
            }
        }

        if (sipMsg.getNumContact() == 1)
        {
            SipRoute route("", getLocalIp());
            route.setUrl(sipMsg.getContact().getUrl());
            setRoute(route);

            //remove the contact header.
            setNumContact(0);
        }
    }

    Sptr <BaseUrl> url = getTo().getUrl();

    SipVia sipvia("", getLocalIp());
    //create the  via header. This contains details present in the toUrl.
    Sptr<BaseUrl> toUrl = getFrom().getUrl();

    //default transport is UDP.
    sipvia.setprotoVersion("2.0");
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(toUrl.getPtr()));
	    sipvia.setHost(sipUrl->getHost());
	    sipvia.setPort(sipUrl->getPort().convertInt());
	}
    }

    //construct the request line.
    SipRequestLine siprequestline(SIP_TRANSFER, url, getLocalIp());
    setRequestLine(siprequestline);

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

    SipRequestLine reqLine = getRequestLine();
    reqLine.setUrl( dest );
    setRequestLine( reqLine );



    //add via to the top
    setVia(sipvia, 0);

    //assign the CallId.
    setCallId(sipMsg.getCallId());

    //create the CSeq header
    SipCSeq cseq = sipMsg.getCSeq();
    int value = cseq.getCSeqData().convertInt();
    //increment cseq.
    value++;

    SipCSeq newcseq(SIP_TRANSFER, Data(value), getLocalIp());
    setCSeq(newcseq);
    //build each header , and store it in that header class.

    //create the callId header
    SipCallId callId("", getLocalIp());
    setCallId(sipMsg.getCallId());

    cpLog(LOG_DEBUG_STACK, "Completed the call id header");


    SipTransferTo transferTo(transferToUrl, getLocalIp());
    setTransferTo(transferTo);

    SipRequire require("", getLocalIp());
    require.set("cc Transfer");
    setRequire(require);

}


TransferMsg::TransferMsg( Data& data, const string& _local_ip )
        : SipCommand(_local_ip, "TransferMsg")
{

    try
    {
        SipCommand::decode(data);
    }
    catch (SipParserException& e)
    {
        cpLog(LOG_ERR, "Failed to parse the SIP msg, since: %s", e.getDescription().c_str());
    }

}    // TransferMsg::TransferMsg( Data& data )


Method 
TransferMsg::getType() const
{
    return SIP_TRANSFER;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
