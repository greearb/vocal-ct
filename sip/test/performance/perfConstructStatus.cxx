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


#include "DurationStatistic.hxx"
#include "PerformanceDb.hxx"
#include "TickCount.hxx"
#include "Data.hxx"
#include "InviteMsg.hxx"
#include "StatusMsg.hxx"
#include "AckMsg.hxx"
#include "TimeTracker.hxx"
#include "SipTransactionGC.hxx"

using Vocal::Statistics::DurationStatistic;
using Vocal::Statistics::StatisticsDb;
using Vocal::Statistics::TickCount;

#define MSG_COUNT 1

#if 0
char* msgtext[MSG_COUNT] = {
    "INVITE sip:5221@192.168.66.104:5060;user=phone SIP/2.0\r\n\
Via: SIP/2.0/UDP 192.168.66.103:5060;branch=1\r\n\
Via: SIP/2.0/UDP 192.168.66.2:5060\r\n\
From: UserAgent<sip:5220@192.168.66.2:5060;user=phone>;\r\n\
To: 5221<sip:5221@192.168.66.103:5060;user=phone>;\r\n\
Call-ID: 209747329338013@192.168.66.2\r\n\
CSeq: 1 INVITE\r\n\
Proxy-Authorization: Basic VovidaClassXswitch\r\n\
Subject: VovidaINVITE\r\n\
Record-Route: <sip:5221@192.168.66.103:5060>\r\n\
Contact: <sip:5220@192.168.66.2:5060;user=phone>\r\n\
Content-Type: application/sdp\r\n\
Content-Length: 165\r\n\
\r\n\
v=0\r\n\
o=- 528534705 528534705 IN IP4 192.168.66.2\r\n\
s=VOVIDA Session\r\n\
c=IN IP4 192.168.66.2\r\n\
t=964134816 0\r\n\
m=audio 23456 RTP/AVP 0\r\n\
a=rtpmap:0 PCMU/8000\r\n\
a=ptime:20\r\n\
\r\n",

};
#endif

char* msgtext[MSG_COUNT] = { 
        "INVITE sip:5452@172.19.174.171:5060 SIP/2.0\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.4\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=d1faef3fff22fbedaa794b8ea2abc7e5.2\r\n\
Via: SIP/2.0/UDP 172.19.174.170:5060\r\n\
From: \"5451\"<sip:5451@172.19.175.150>\r\n\
To: <sip:5452@172.19.175.150>\r\n\
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
\r\n",

};     

int
main( int argc, char* argv[] )
{
    if ( argc < 2 )
    {
        cerr << "Usage: "
        << argv[0]
	<< " <#messagesToParse>"
	<< endl;
        exit( -1 );
    }
    
     int numMessages = 1; 
    
    if (argc == 2)
    {
	numMessages = atoi(argv[1]);
    }
    
    Data data(msgtext[0]);
 
    
    //StatisticsDb db;
    //DurationStatistic stat(db, Data("ParseInvite"));

    
    Data data2 = data;
    
    InviteMsg invite(data2);
    TimeTracker t(cerr);
    t.startTime();
    for (int i = 0; i< numMessages; i++)
    {
        //stat.start();
        StatusMsg stmsg(invite, 100);
        //stat.stop();
        //stat.record();
        //AckMsg ackmsg(stmsg);
        //cerr  << "Invite: " << invite.encode().getData() << endl;
        //cerr  << "+++++++++++++++" << endl;
        //cerr  << "Status: " << stmsg.encode().getData() << endl;
    }
    t.endTime();

    //cout << "average ticks : " << stat.average() << endl;
    
    return 0;
}
