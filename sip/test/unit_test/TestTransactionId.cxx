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

static const char* const TestTransactionId_cxx_Version =
    "$Id: TestTransactionId.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "VTest.hxx"
#include "SipMsg.hxx"
#include "SipTransactionId.hxx"
#include "Verify.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


#define MSG_COUNT 2

char* msgtext[MSG_COUNT] = {
    "\
    SIP/2.0 200 OK\r\n
    Via: SIP/2.0/UDP 192.168.5.3:5060\r\n
    From: 6398<sip:6398@192.168.5.3:5060>\r\n
    To: 6500<sip:6500@192.168.5.4:5060>\r\n
    Call-ID: 920569947105795@192.168.5.3\r\n
    CSeq: 1 INVITE\r\n
    Contact: <sip:6500@192.168.5.4:5060>\r\n
    Content-Type: application/sdp\r\n
    Content-Length: 130\r\n
    \r\n
    v=0\r\n
    o=- 1274535136 1274535136 IN IP4 192.168.5.4\r\n
    s=VOVIDA Session\r\n
    c=IN IP4 192.168.5.4\r\n
    t=963964446 0\r\n
    m=audio 3456 RTP/AVP 0\r\n
    \r\n",
    "\
    ACK sip:6500@192.168.5.4:5060 SIP/2.0\r\n
    Via: SIP/2.0/UDP 192.168.5.3:5060\r\n
    From: 6398<sip:6398@192.168.5.3:5060>\r\n
    To: 6500<sip:6500@192.168.5.4:5060>\r\n
    Call-ID: 920569947105795@192.168.5.3\r\n
    CSeq: 1 ACK\r\n
    Content-Length: 0\r\n
    \r\n
    ",
};

SipMsg* msg[MSG_COUNT];
SipTransactionId* id[MSG_COUNT];

void init_msg()
{
    for (int i = 0; i < MSG_COUNT; i++)
    {
	Data rawData = Data(msgtext[i]);
    
        msg[i] = SipMsg::decode(rawData, host_addr);
        id[i] = new SipTransactionId(*msg[i]);
    }
}

bool test_00()
{
    return !(*id[0] == *id[1]);
}

#if 0
bool test_01()
{
    SipTransactionId idTmp(id[1]);
//    idTmp.replaceCSeqMethod(SIP_INVITE);
    bool ok = (id[0] == idTmp);

    if (!ok)
    {
//        id[0].printComparison(idTmp);
//        cpLog(LOG_DEBUG, "0: \n%s\n", msg[0]->encode().logData());
//        cpLog(LOG_DEBUG, "1: \n%s\n", msg[1]->encode().logData());
    }
    return ok;
}
#endif

int main()
{
    init_msg();
//    cpLogSetPriority(LOG_DEBUG_STACK);

    test_verify(test_00());
//    test_verify(test_01());
    return test_return_code(1);
}
