
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

static const char* const TransceiverDBase_cxx_Version =
    "$Id: TransceiverDBase.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <Data.hxx>
#include <cpLog.h>
#include <parse3tuple.h>
#include "InviteMsg.hxx"
#include "StatusMsg.hxx"
#include "AckMsg.hxx"
#include "ByeMsg.hxx"
#include "CancelMsg.hxx"
#include "RegisterMsg.hxx"
#include "SipTransceiver.hxx"
#include "SipContact.hxx"
#include "SipContact.hxx"
#include "Sptr.hxx"
#include "SipSentReqRecvRespDBase.hxx"
#include "SipSentRespRecvReqDBase.hxx"
#include "SipMsgQueue.hxx"

int main()
{
    //The url is :
    //goofy@vovida.com
    cpLogSetLabel ("Invitetest");
    cpLogSetPriority (LOG_DEBUG_STACK);

    parse3tuple ("SystemInfo.cfg", parseSystemInfo);

    /**  Data url = " INVITE sip:goofy@vovida.com SIP/2.0\r\n";
      url+= "Via: SIP/2.0/UDP csvax.cs.caltech.edu;branch=8348\r\n";
      url+= "From: goofy <sip:mjh@isi.edu>\r\n";
      url+= "To: goofy2<sip:schooler@caltech.edu>\r\n";
      url+= "Call-ID: 234567\r\n";
      url+= "CSeq: 1 INVITE\r\n";
      url+= "Subject: SIP..\r\n";
      url+= "Content-Type: application/sdp\r\n";
      url+= "Content-Length:187\r\n";
      url+= "\r\n";
      url+= "v=0\r\n";
      url+= "o=user1 3456 1234 IN IP4 128.3.4.5\r\n";
      url+= "c=IN IP4 128.3.4.5\r\n";
      url+= "m=audio 3456 RTP/AVP 0\r\n";
    */


    //if the above works, try this:

    //Data url = "goofy:goofy_password@192.168.5.12:5060;transport=udp;user=phone;tag=4567";

    Data url;
    //send invite to lab2
    url = "sip:destination@192.168.5.130";
    Sptr <BaseUrl> tourl;
    tourl = BaseUrl::decode(url);

    if (tourl.getPtr() != 0)
    {
	if (tourl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
    
	    string h = (sipUrl.dynamicCast(tourl))->getHost();
	    string p = (sipUrl.dynamicCast(tourl))->getPort();
	    cout << "host is " << h << endl;
	    cout << "port is " << p << endl;
	}
    }

    int listenPort = 5060;
    int rtpPort = 3456;
    cpLog(LOG_DEBUG, "Completed the Url decoding");
    Sptr < InviteMsg > invite = new InviteMsg(tourl, listenPort, rtpPort);



    SipSentReqRecvRespDBase sentreq;
    sentreq.insertSentReq(invite);


    Sptr < StatusMsg > status = new StatusMsg(*invite, 200);
    Sptr < SipMsgQueue > msgQueue = sentreq.retreiveEntireTrans(status);


    Sptr < CancelMsg > cancelMsg = new CancelMsg(*invite);

    SipTransactionId id(*cancelMsg);

    sentreq.clearSentReq(id);


    cpLog(LOG_DEBUG, "msgs are: ");

    SipMsgQueue::iterator i;
    i = msgQueue->begin();
    while (i != msgQueue->end())
    {

        cout << (*i)->encode().logData() << endl;
        ++i;
    }


    cpLog(LOG_DEBUG, "SipSentRespRecvReqtests");
    SipSentRespRecvReqDBase sentresp;
    sentresp.insertRecvdReq(invite);


    cpLog(LOG_DEBUG, " inserted the invite");

    sentresp.insertSentResp(status);


    cpLog(LOG_DEBUG, " inserted the status");

    Sptr < AckMsg > ackMsg = new AckMsg(*status);


    cpLog(LOG_DEBUG, " constructed the AckMsg");

    Sptr < SipMsgQueue > ackQueue = sentresp.retreiveEntireTrans(ackMsg);
    if (ackQueue != 0)
    {
        cpLog(LOG_DEBUG, "ackQueue not empty");
        SipMsgQueue::iterator it;
        it = ackQueue->begin();
        while (it != ackQueue->end())
        {
            cout << (*it)->encode().logData() << endl;
            ++it;
        }
    }

    return 1;
}
