
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

static const char* const SipFilterDbaseMemory_cxx_Version =
    "$Id: SipFilterDbaseMemory.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "VTest.hxx"
#include "SipMsg.hxx"
#include "SipFilterReceiveDBase.hxx"
#include "SipTransceiver.hxx"
#include "InviteMsg.hxx"
#include "SipCallId.hxx"
#include "Data.hxx"
#include <deque>
#include <list>

#define MSG_COUNT 1

char* msgtext[MSG_COUNT] = {
    "\
    INVITE sip:5221@192.168.66.104:5060;user=phone SIP/2.0\r
    Via: SIP/2.0/UDP 192.168.66.103:5060;branch=1\r
    Via: SIP/2.0/UDP 192.168.66.2:5060\r
    From: UserAgent<sip:5220@192.168.66.2:5060;user=phone>;\r
    To: 5221<sip:5221@192.168.66.103:5060;user=phone>;\r
    Call-ID: 209747329338013@192.168.66.2\r
    CSeq: 1 INVITE\r
    Proxy-Authorization: Basic VovidaClassXswitch\r
    Subject: VovidaINVITE\r
    Record-Route: <sip:5221@192.168.66.103:5060>\r
    Contact: <sip:5220@192.168.66.2:5060;user=phone>\r
    Content-Type: application/sdp\r
    Content-Length: 165\r
    \r
    v=0\r
    o=- 528534705 528534705 IN IP4 192.168.66.2\r
    s=VOVIDA Session\r
    c=IN IP4 192.168.66.2\r
    t=964134816 0\r
    m=audio 23456 RTP/AVP 0\r
    a=rtpmap:0 PCMU/8000\r
    a=ptime:20\r
    \r",

};

typedef list < Sptr < SipMsg > > Queue;

struct junk
{
    Sptr < SipMsg > data;
};

list < int > event_queue;
int constructor = 0;
int copy_constructor = 0;
int destructor = 0;

class X
{
    public:
        X()
        {
            //	event_queue.push_back(1);
        }

        X(const X& x)
        {
            //	event_queue.push_back(2);
        }

        ~X()
        {
            //	event_queue.push_back(3);
        }

};

typedef X Y;

typedef list < Y > Queue2;

bool test_00()
{
    SipFilterReceiveDBase dbase;
    Data data = msgtext[0];
    InviteMsg invite(data);

    Queue* queue;
    queue = new Queue;

    Queue2* queue2;
    queue2 = new Queue2;

    for (int i = 0; i < 1000; i++)
    {
        SipCallId id;
        invite.setCallId( id );

        Sptr < SipMsg > add = copyPtrtoSptr(&invite);
        //	Sptr<SipMsg> result;

        junk* foo = new junk;

        new (&foo->data) Sptr < SipMsg > (add);

        (&foo->data)->~Sptr();

        X my_x;
        queue2->push_back(my_x);

        delete foo;

        //	queue->push_back(junk);

        //	dbase.findOrInsert(add, &result);
    }


    queue->clear();
    queue2->clear();
    delete queue;
    delete queue2;

    return true;
}

int main()
{
    cpLogSetPriority(LOG_INFO);

    test_00();
    return 0;
}
