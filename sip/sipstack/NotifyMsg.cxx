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

static const char* const NotifyMsg_cxx_Version =
    "$Id: NotifyMsg.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "NotifyMsg.hxx"
#include "SipCallLeg.hxx"
#include "SipMsg.hxx"
#include "SipRoute.hxx"
#include "SipSubsNotifyEvent.hxx"
#include "SipVia.hxx"
#include "SubscribeMsg.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"
#include "symbols.hxx"
#include "SipUnknownContentData.hxx"

using namespace Vocal;


NotifyMsg::NotifyMsg(const string& local_ip)
        : SipCommand(local_ip)
{
    myRequestLine.setMethod(NOTIFY_METHOD);
    SipCSeq cseq( SIP_NOTIFY, 0, local_ip );
    setCSeq( cseq );
}


NotifyMsg::NotifyMsg(const Data& data, const string& local_ip)
        : SipCommand(local_ip)
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

NotifyMsg::NotifyMsg(const SubscribeMsg& subscribeRequest, 
		     const SipSubsNotifyEvent& event,
                     const string& local_ip)
        : SipCommand(local_ip)
{
    //set call-id, from, and to
    setCallId(subscribeRequest.getCallId());
    SipFrom newFrom(subscribeRequest.getTo());
    SipTo newTo(subscribeRequest.getFrom());
    newFrom.setTag(subscribeRequest.getTo().getTag());
    newTo.setTag(subscribeRequest.getFrom().getTag());
    setTo(newTo);
    setFrom(newFrom);

    //make sure no SubNotifyEvent headers exist in the new NOTIFY message
    flushSubsNotifyEvent();

    bool exists = false;
    for(int i = 0; i < subscribeRequest.getNumSubsNotifyEvent(); i++)
    {
	if(event == subscribeRequest.getSubsNotifyEvent(i))
	{
	    exists = true;
	}
    }
    if(exists)
    {
        setSubsNotifyEvent(event);
    }

    //set the other headers of NOTIFY message
    Sptr <BaseUrl> fromUrl = subscribeRequest.getFrom().getUrl();
    setNotifyDetails(fromUrl);
}


NotifyMsg::NotifyMsg(const SipCallLeg& callLeg, const SipSubsNotifyEvent& event,
                     const string& local_ip)
        : SipCommand(local_ip)
{
    //set call-id, from, and to
    setCallId(callLeg.getCallId());
    setTo(callLeg.getTo());
    setFrom(callLeg.getFrom());

    //make sure no SubNotifyEvent headers exist in the new NOTIFY message
    flushSubsNotifyEvent();
    setSubsNotifyEvent(event);

    //set the other headers of NOTIFY message
    Sptr <BaseUrl> fromUrl = callLeg.getFrom().getUrl();
    setNotifyDetails(fromUrl);
}


NotifyMsg::NotifyMsg(const NotifyMsg& src)
        : SipCommand(src)
{
}


NotifyMsg::~NotifyMsg()
{
}


const NotifyMsg& 
NotifyMsg::operator=(const NotifyMsg& src)
{
    if ( &src != this )
    {
        SipCommand::operator=(src);
    }
    return (*this);
}
    
    
bool 
NotifyMsg::operator ==(const NotifyMsg& src)
{
    return ( *static_cast <SipCommand*> (this) == src );
}
        

Vocal::Method 
NotifyMsg::getType() const
{
    return SIP_NOTIFY;
}

SipStatusLine
NotifyMsg::getReferredStatusLine() const
{
    if (getNumSubsNotifyEvent() == 1)
    {
        Data nevent = getSubsNotifyEvent(0).getEventType();
        Data result = nevent.matchChar(";");
        
        if (result == "refer" || nevent == "refer")
        {
            Sptr<SipContentData> content = getContentData(0);
            Sptr<SipUnknownContentData> text;
            text.dynamicCast(content);
            if (text != 0)
            {
                int i;
                cpLog(LOG_DEBUG_STACK, "output body: <%s> status=<%s>", 
                      text->encodeBody(i).logData(),
                      text->encodeBody(i).getLine().logData());
                Data frag = text->encodeBody(i);
                // !jf! this will not work if fragment contains anything other than
                // status line
                return SipStatusLine(frag);
            }
            else
            {
                cpLog(LOG_DEBUG, "no reffered status line in %s", encode().logData());
                throw NotReferException(__FILE__, __LINE__);
            }
        }
        else
        {
            cpLog(LOG_DEBUG, "no reffered status line in %s", encode().logData());
            throw NotReferException(__FILE__, __LINE__);
        }
    }
    return SipStatusLine();
}

NotifyMsg::NotifyMsg(const NotifyMsg& msg, enum NotifyMsgForward)
    : SipCommand(msg)
{
    *this = msg;

    Sptr <BaseUrl> dest = getTo().getUrl();

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
   //  myRequestLine.setUrl(dest);
//     SipRequestLine reqLine = getRequestLine();
//     reqLine.setUrl( dest );
//     setRequestLine( reqLine );
    myRequestLine.setUrl(dest);
}


void 
NotifyMsg::setNotifyDetails(Sptr <BaseUrl>  fromUrl)
{
    //set the CSeq
    SipCSeq cseq(Vocal::SIP_NOTIFY, Data(1), getLocalIp());
    setCSeq(cseq);

    //set the Via from host creating NOTIFY msg
    SipVia sipVia("", getLocalIp());
    sipVia.setprotoVersion("2.0");
    sipVia.setHost(Data(getLocalIp()));
    setVia(sipVia);

    //set the request line
    SipRequestLine requestLine(SIP_NOTIFY, fromUrl, getLocalIp());
    setRequestLine(requestLine);
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
