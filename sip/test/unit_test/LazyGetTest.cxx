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

static const char* const LazyGetTest_cxx_Version =
    "$Id: LazyGetTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipAuthorization.hxx"
#include "symbols.hxx"
#include <cassert>
#include <sys/time.h>
#include "cpLog.h"
#include "InviteMsg.hxx"
#include "Verify.hxx"
#include "SipAccept.hxx"
#include "SipUserAgent.hxx"
#include "SipVia.hxx"
#include "SipRecordRoute.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;

    
    
char msgtext[] = {
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
    
void testGet()
{
    Data invitedata(msgtext);
    InviteMsg invite(invitedata, host_addr);
    
    SipRequestLine reqLine = invite.getRequestLine();    
    Data reqEncode = reqLine.encode();
    cout << "request Line: " << reqEncode.logData() << endl;
    
    test_verify (Data("INVITE sip:5452@172.19.174.171:5060 SIP/2.0\r\n") ==  reqEncode );

    int numVia = invite.getNumVia();
    for (int i = 0; i < numVia; i++)
    {
	Data viaEncode = invite.getVia(i).encode();
	cout << "via line: " << viaEncode.logData() << endl;
        if (i == 0)
	{
	    test_verify(Data("Via: SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.4\r\n") == viaEncode );
	}
	else if (i == 1)
	{
	    test_verify (Data("Via: SIP/2.0/UDP 172.19.175.150:5060;branch=d1faef3fff22fbedaa794b8ea2abc7e5.2\r\n") == viaEncode );
	}
	else if (i ==2)
	{
	   test_verify (Data("Via: SIP/2.0/UDP 172.19.174.170:5060\r\n") == viaEncode ); 
	}
    }
    
    SipFrom fromLine = invite.getFrom();
    Data fromEncode = fromLine.encode();
    cout << "from line: " << fromEncode.logData() << endl;
    
    test_verify (Data("From: \"5451\"<sip:5451@172.19.175.150>\r\n") == fromEncode);
    
    SipTo toLine = invite.getTo();
    Data toEncode = toLine.encode();
    cout << "to line: " << toEncode.logData() << endl;
    
    test_verify (Data("To: <sip:5452@172.19.175.150>\r\n") == toEncode);
    
    SipCallId callidLine = invite.getCallId();
    Data callidEncode = callidLine.encode();
    cout << "callid line:" << callidEncode.logData() << endl;
    
    test_verify (Data("Call-ID: c4943000-75748-7058-2e323731@172.19.174.170\r\n") == callidEncode);
    
    SipCSeq cseqLine = invite.getCSeq();
    Data cseqEncode = cseqLine.encode();
    cout << "cseq Line:" << cseqEncode.logData() << endl;
    
    test_verify (Data("CSeq: 101 INVITE\r\n") == cseqEncode);
    
    SipExpires expiresLine = invite.getExpires();
    Data expiresEncode = expiresLine.encode();
    cout << "expires Line:" << expiresEncode.logData() << endl;
    
    test_verify (Data("Expires: 180\r\n") == expiresEncode);
    
    SipAccept acceptLine = invite.getAccept(0);
    Data acceptEncode = acceptLine.encode();
    cout << "accept Line:" << acceptEncode.logData() << endl;
    
    test_verify (Data("application/sdp") == acceptEncode);
    
    int numRRoute = invite.getNumRecordRoute();
    
    for (int j = 0; j < numRRoute; j++)
    {
	SipRecordRoute recordRouteLine = invite.getRecordRoute(j);
	Data recRouteEncode = recordRouteLine.encode();
	cout << " recRoute Line:" << recRouteEncode.logData() << endl;
    
	if (j == 0)
	{
	    test_verify (Data("<sip:5452@172.19.175.150:5060;maddr=172.19.175.150>") == recRouteEncode);
        }
    
	else if (j == 1)
	{
	    test_verify (Data("<sip:5452@172.19.175.150:5060;maddr=172.19.175.150>") == recRouteEncode);
        }
	
    }
    
    SipContact contactLine = invite.getContact(0);
    Data contactEncode = contactLine.encode();
    cout << " contact Line:" << contactEncode.logData() << endl;
    
    test_verify (Data("Contact: <sip:5451@172.19.174.170:5060>\r\n") == contactEncode);

    SipContentLength lengthLine = invite.getContentLength();
    Data lengthEncode = lengthLine.encode();
    cout << " conten length:" << lengthEncode.logData() << endl;
    
    test_verify (Data("Content-Length: 228\r\n") == lengthEncode);
    
    SipUserAgent uaLine = invite.getUserAgent();
    Data uaEncode = uaLine.encode();
    cout << "user agent line:" << uaEncode.logData() << endl;
    
    test_verify (Data("User-Agent: Cisco IP Phone/ Rev. 1/ SIP enabled\r\n") == uaEncode);

    SipContentType typeLine = invite.getContentType();
    Data typeEncode = typeLine.encode();
    cout << "content type:" << typeEncode.logData() << endl;
    
    test_verify (Data("Content-Type: application/sdp\r\n") == typeEncode);
    
    
    
    Sptr<SipContentData> dataLine = invite.getContentData(0);
    int k;
    
    Data dataEncode = dataLine->encodeBody(k);
    
    cout << "data:" << dataEncode.logData() << endl;

    test_verify (Data("v=0\r\n\
o=CiscoSystemsSIP-IPPhone-UserAgent 21803 10790 IN IP4 172.19.174.170\r\n\
s=SIP Call\r\n\
c=IN IP4 172.19.174.170\r\n\
t=0 0\r\n\
m=audio 28844 RTP/AVP 0 8 18 100\r\n\
a=rtpmap:0 pcmu/8000\r\n\
a=rtpmap:100 telephone-event/8000\r\n\
a=fmtp:100 0-11\r\n") == dataEncode);
}


int main()
{
//    cpLogSetPriority(LOG_DEBUG_STACK);

    testGet();
    
    return test_return_code(17);
}
