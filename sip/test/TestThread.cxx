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

static const char* const TestThread_cxx_Version =
    "$Id: TestThread.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "ThreadIf.hxx"
#include "SipMsg.hxx"
#include "SipMsgQueue.hxx"
#include "Fifo.h"

using namespace std;
using namespace Vocal;

const char* invite =
        "INVITE sip:5452@172.19.174.171:5060;user=phone SIP/2.0\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.4\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=d1faef3fff22fbedaa794b8ea2abc7e5.2\r\n\
Via: SIP/2.0/UDP 172.19.174.170:5060\r\n\
From: \"5451\"<sip:5451@172.19.175.150>\r\n\
To: <sip:5452@172.19.175.150;user=phone>\r\n\
Call-ID: c4943000-75748-7058-2e323731@172.19.174.170\r\n\
CSeq: 101 INVITE\r\n\
Expires: 180\r\n\
Accept: application/sdp\r\n\
Record-Route: <sip:5452@172.19.175.150:5060;maddr=172.19.175.150>,<sip:5452@172.19.175.150:5060;maddr=172.19.175.150>\r\n\
Contact: <sip:5451@172.19.174.170:5060>\r\n\
Content-Length: 228\r\n\
User-Agent: Cisco IP Phone/ Rev. 1/ SIP enabled\r\n\
Content-Type: application/sdp\r\n\
\r\n\
v=0\r\n\
o=CiscoSystemsSIP-IPPhone-UserAgent 21803 10790 IN IP4 172.19.174.170\r\n\
s=SIP Call\r\n\
c=IN IP4 172.19.174.170\r\n\
t=0 0\r\n\
m=audio 28844 RTP/AVP 0 8 18 100\r\n\
a=rtpmap:0 pcmu/8000\r\n\
a=rtpmap:100 telephone-event/8000\r\n\
a=fmtp:100 0-11\r\n\
\r\n";




class Event
{
    public:
	Sptr < SipMsg > mySipMsg;
        Sptr < SipMsgQueue > mySipMsgQueue;
};

Fifo < Event > myFifo;

Fifo < Sptr<SipMsg > > gcFifo;

class Worker : public ThreadIf
{
    public:
	void thread()
	{
	    int count = 0;
	    while ( 1 )
	    {
		Event nextEvent = myFifo.getNext();
		nextEvent.mySipMsg->getTo();
		nextEvent.mySipMsg->getContact();
		++count;
		if(count % 25000 == 0)
		{
		    cout << count << "\n";
		}
	    }
	}
};


class GC : public ThreadIf
{
    public:
	void thread()
	{
	    while(1)
	    {
		Sptr<SipMsg> msg = gcFifo.getNext();

	    }
	}
};


class Creator : public ThreadIf
{
    public:
	void thread()
	{
	    while(1)
	    {
		Sptr<SipMsg> msg = SipMsg::decode(invite);
//		gcFifo.addDelay(msg, 5000);
		gcFifo.add(msg);

		Sptr<SipMsgQueue> q = new SipMsgQueue;

		q->push_back(msg);
		Event a;
		a.mySipMsg = msg;
		a.mySipMsgQueue = q;
		myFifo.add(a);
	    }
	}

};




int main()
{
    Creator a;
    GC b;
    Worker c;

    a.run();
    b.run();
    c.run();

    a.join();
    b.join();
    c.join();

}
