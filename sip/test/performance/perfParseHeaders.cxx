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
#include "SipUserAgent.hxx"
#include "SipSdp.hxx"
#include "SipVia.hxx"
#include "SipRecordRoute.hxx"

using namespace Vocal;
using namespace Vocal::SDP;

using Vocal::Statistics::DurationStatistic;
using Vocal::Statistics::StatisticsDb;
using Vocal::Statistics::TickCount;

using namespace Vocal;

#define MSG_COUNT 1
    
#define NONE 0
#define FROM 1
#define TO 2
#define VIA 3
#define RECROUTE 4
#define CALLID 5
#define CSEQ 6
#define CONTENTTYPE 7
#define CONTENTLENGTH 8
#define CONTENTDATA 9
#define EXPIRES 10
#define CONTACT 11
#define USERAGENT 12
#define REQLINE 13
#define URL 14
#define DATE 15    
    


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
    if ( argc < 3 )
    {
        cerr << "Usage: "
        << argv[0]
	<< "<#messagesToParse>"
	<< "<HeaderName:\r\nEnter the number corresponding to the Header:\r\nFrom-1, To-2, Via-3, RecRoute-4, CallId-5, CSeq-6, ContentType-7\r\n ContentLength-8, ContentData-9, Expires-10, Contact-11, UserAgent-12, ReqLine-13,Url-14, Date-15>" << endl;
        exit( -1 );
    }
    
    int numMessages = 1;
    
    int headerSelect = NONE;
    
    
    if (argc >= 2)
    {
	numMessages = atoi(argv[1]);
    }
    
    if (argc >= 3)
    {
        headerSelect = atoi(argv[2]); 
    }
    
    Data data(msgtext[0]);
 
    StatisticsDb db;
    
    DurationStatistic stat(db, Data("ParseHeaders"));

    
    // InviteMsg invite(data);
    
    float sum = 0;
    
  
    
    stat.start();
    for (int i = 0; i< numMessages; i++)
    {  
        switch(headerSelect)
	{
	    case FROM:
	    {
	        SipFrom from("\"5451\"<sip:5451@172.19.175.150>");   
	    }
	    break;
            case TO:
	    {
		SipTo to("<sip:5452@172.19.175.150>");
	    }
	    break;
	    case VIA:
	    {
		SipVia via("SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77");
	    }
	    break;
	    case RECROUTE:
	    {
		SipRecordRoute recRoute("<sip:5452@172.19.175.150:5060;maddr=172.19.175.150");
	    }
	    break;
	    case CALLID:
	    {
		SipCallId id("c4943000-75748-7058-2e323731@172.19.174.17");
	    }
	    break;
	    case CSEQ:
	    {
		SipCSeq cseq(" 101 INVITE");
	    }
	    break;
	    case EXPIRES:
	    {
		SipExpires expires("180");
	    }
	    break; 
	    case CONTENTTYPE:
	    {
		SipContentType type("application/sdp");
	    }
	    break;
	    case CONTENTDATA:
	    {
	       
		SdpSession session;
                session.decode("v=0\r\n\
o=CiscoSystemsSIP-IPPhone-UserAgent 21803 10790 IN IP4 172.19.174.170\r\n\
s=SIP Call\r\n\
c=IN IP4 172.19.174.170\r\n\
t=0 0\r\n\
m=audio 28844 RTP/AVP 0 8 18 100\r\n\
a=rtpmap:0 pcmu/8000\r\n\
a=rtpmap:100 telephone-event/8000\r\n\
a=fmtp:100 0-11\r\n\
\r\n");
	    }
	    break;
	    case CONTENTLENGTH:
	    {
		SipContentLength("228");
	    }
	    break;
	    case CONTACT:
	    {
		SipContact("<sip:5451@172.19.174.170:5060>");
	    }
	    break;
	    case USERAGENT:
	    {
		SipUserAgent uagent("Cisco IP Phone/ Rev. 1/ SIP enabled");
	    }
	    break;
	    case REQLINE:
	    {
		SipRequestLine("INVITE sip:5452@172.19.174.171:5060 SIP/2.0");
	    }
	    break;
	    case URL:
	    {
		SipUrl url(Data("sip:5452@172.19.175.150:5060"));
	    }
	    break;
	    case DATE:
	    {
		SipDate dte("Thu, 01 Dec 1994 16:00:00 GMT");
	    }
	    break;
	}
    }
    
    
    stat.stop();
    stat.record();
    sum+= stat.average();
    
    cout << "average ticks : " << sum/numMessages << endl;
    
    return 0;
}
