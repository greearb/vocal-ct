
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

static const char* const SipFilterReceiveDBaseTest_cxx_Version =
    "$Id: SipFilterReceiveDBaseTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "VTest.hxx"
#include "SipMsg.hxx"
#include "SipFilterReceiveDBase.hxx"
#include "SipTransceiver.hxx"

#define MSG_COUNT 3

char* msgtext[MSG_COUNT] = {
    "\
    SIP/2.0 200 OK\r
    Via: SIP/2.0/UDP 192.168.5.3:5060\r
    From: 6398<sip:6398@192.168.5.3:5060>\r
    To: 6500<sip:6500@192.168.5.4:5060>\r
    Call-ID: 920569947105795@192.168.5.3\r
    CSeq: 1 INVITE\r
    Contact: <sip:6500@192.168.5.4:5060>\r
    Content-Type: application/sdp\r
    Content-Length: 130\r
    \r
    v=0\r
    o=- 1274535136 1274535136 IN IP4 192.168.5.4\r
    s=VOVIDA Session\r
    c=IN IP4 192.168.5.4\r
    t=963964446 0\r
    m=audio 3456 RTP/AVP 0\r
    ",
    "\
    ACK sip:6500@192.168.5.4:5060 SIP/2.0\r
    Via: SIP/2.0/UDP 192.168.5.3:5060\r
    From: 6398<sip:6398@192.168.5.3:5060>\r
    To: 6500<sip:6500@192.168.5.4:5060>\r
    Call-ID: 920569947105795@192.168.5.3\r
    CSeq: 1 ACK\r
    Content-Length: 0\r
    \r
    ",
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

SipMsg* msg[MSG_COUNT];
SipTransactionId id[MSG_COUNT];

void init_msg()
{
    for (int i = 0; i < MSG_COUNT; i++)
    {
        msg[i] = SipMsg::decode(Data(msgtext[i]));
        id[i] = SipTransactionId(*msg[i]);
    }
}

bool test_00()
{
    SipFilterReceiveDBase dbase;

    Sptr < SipMsg > msg0 = copyPtrtoSptr(msg[0]);
    Sptr < SipMsg > msg1 = copyPtrtoSptr(msg[1]);

    Sptr < SipMsg > returnItem;

    bool retval = dbase.findOrInsert(msg0, &returnItem);

    if (retval || (returnItem != 0))
    {
        return false;
    }

    retval = dbase.findOrInsert(msg0, &returnItem);
    if (!retval || (returnItem != 0))
    {
        return false;
    }

    dbase.insertSent(msg1);

    retval = dbase.findOrInsert(msg0, &returnItem);
    if (!retval || (returnItem == 0) || (*returnItem != *msg1))
    {
        return false;
    }
    return true;
}


bool test_01()
{
    SipFilterReceiveDBase dbase;

    Sptr < SipMsg > msg2 = copyPtrtoSptr(msg[2]);

    Sptr < SipMsg > returnItem;

    bool retval = dbase.findOrInsert(msg2, &returnItem);

    if (!retval)
    {
        Sptr < SipCommand > x;
        x.dynamicCast(msg2);
        dbase.erase(x);
    }

    return true;
}

int main()
{
    init_msg();
    cpLogSetPriority(LOG_DEBUG_STACK);
    VTest test("SipFilterReceiveDBaseTest");

    test.test(0, test_00(), "test 0");
    test.test(1, test_01(), "test 1");
    return test.exitCode();
}
