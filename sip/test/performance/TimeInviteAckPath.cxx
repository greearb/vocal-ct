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

static const char* const TimeInviteAckPath_cxx_Version = 
    "$Id: TimeInviteAckPath.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "DurationStatistic.hxx"
#include "PerformanceDb.hxx"
#include "Data.hxx"
#include "InviteMsg.hxx"
#include "StatusMsg.hxx"
#include "AckMsg.hxx"
#include "StatisticsDb.hxx"

using namespace Vocal;


using Vocal::Statistics::DurationStatistic;
using Vocal::Statistics::StatisticsDb;
using Vocal::Statistics::TickCount;

#define MSG_COUNT 1

char* msgtext[] = {
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
\r
",
    0
};

inline 
void code()
{
    Sptr<SipMsg> msg = SipMsg::decode(Data(msgtext[0]));
    Sptr<InviteMsg> invite;
    invite.dynamicCast(msg);
    StatusMsg status(*invite, 200);
//    cout << status.encode() << endl;
}

int main(int argc, char** argv)
{
    int count = atoi(argv[1]);

    StatisticsDb db;
    
    DurationStatistic stat(db, Data("ParseInvite"));

    stat.start();
    for(int i = 0; i < count ; ++i)
    {
	code();
    }
    stat.stop();
    stat.record();
    cout << "average ticks : " << stat.average() << endl;

    return 0;
}
