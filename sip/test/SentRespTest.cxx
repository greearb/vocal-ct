
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

static const char* const SentRespTest_cxx_Version =
    "$Id: SentRespTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "SipSentRespRecvReqDBase.hxx"
#include "SipMsg.hxx"
#include "Data.hxx"
#include <iostream>
#include "SipMsgQueue.hxx"
#include "cpLog.h"

const char* msg0 = "INVITE sip:6388@192.168.5.130:5060 SIP/2.0\r
                   Via: SIP/2.0/UDP 192.168.5.130:5060\r
                   From: UserAgent<sip:6388@192.168.5.130:5060>\r
                   To: 93831073<sip:93831073@192.168.16.180:5060;user=phone>\r
                   Call-ID: 966752985@192.168.5.130\r
                   CSeq: 1 INVITE\r
                   Contact: <sip:93831073@192.168.6.210:5060;user=phone>\r
                   Record-Route: <sip:93831073@192.168.16.110:5060>,<sip:93831073@192.168.16.180:5060>\r
                   Content-Type: application/sdp\r
                   Content-Length: 135\r

                   v=0\r
                   o=CiscoSystemsSIP-GW-UserAgent 697 4016 IN IP4 192.168.6.210\r
                   s=SIP Call\r
                   c=IN IP4 192.168.6.210\r
                   t=0 0\r
                   m=audio 20970 RTP/AVP 0\r
                   \0";


const char* msg1 = "SIP/2.0 200 OK\r
                   Via: SIP/2.0/UDP 192.168.5.130:5060\r
                   From: UserAgent<sip:6388@192.168.5.130:5060>\r
                   To: 93831073<sip:93831073@192.168.16.180:5060;user=phone>;tag=39583C-509\r
                   Call-ID: 966752985@192.168.5.130\r
                   CSeq: 1 INVITE\r
                   Contact: <sip:93831073@192.168.6.210:5060;user=phone>\r
                   Record-Route: <sip:93831073@192.168.16.110:5060>,<sip:93831073@192.168.16.180:5060>\r
                   Content-Type: application/sdp\r
                   Content-Length: 135\r

                   v=0\r
                   o=CiscoSystemsSIP-GW-UserAgent 697 4016 IN IP4 192.168.6.210\r
                   s=SIP Call\r
                   c=IN IP4 192.168.6.210\r
                   t=0 0\r
                   m=audio 20970 RTP/AVP 0\r
                   \0";

const char* msg2 = "ACK sip:93831073@192.168.16.180:5060 SIP/2.0\r
                   Via: SIP/2.0/UDP 192.168.5.130:5060\r
                   From: UserAgent<sip:6388@192.168.5.130:5060>\r
                   To: 93831073<sip:93831073@192.168.16.180:5060;user=phone>;tag=39583C-509\r
                   Call-ID: 966752985@192.168.5.130\r
                   CSeq: 1 ACK\r
                   Route: <sip:93831073@192.168.16.110:5060>,<sip:93831073@192.168.6.210:5060>\r
                   Content-Length: 0\r
                   \r
                   \0";

int main()
{

    cpLogSetPriority(LOG_DEBUG_STACK);
#if 0
    cout << msg0 << endl;
    cout << msg1 << endl;
    cout << msg2 << endl;
#endif

    Data l(msg0);
    Data m(msg1);
    Data n(msg2);

    Sptr < StatusMsg > response;
    response.dynamicCast(Sptr < SipMsg > (SipMsg::decode(m)));

    Sptr < SipCommand > ack;
    ack.dynamicCast(Sptr < SipMsg > (SipMsg::decode(n)));

    Sptr < SipCommand > invite;
    invite.dynamicCast(Sptr < SipMsg > (SipMsg::decode(l)));
#if 0
    cout << "begin" << endl;

    cout << response->encode().logData() << endl << endl;
    cout << ack->encode().logData() << endl << endl;
#endif

    cout << "dbing..." << endl;

    SipSentRespRecvReqDBase foo;

    foo.insertRecvdReq(invite);

    foo.insertSentResp(response);

    Sptr < SipMsgQueue > x = foo.retreiveEntireTrans(ack);

    if (x != 0)
    {
        cout << "ok?" << endl;
    }
    else
    {
        cout << "fail?" << endl;
    }


    return 0;
}
