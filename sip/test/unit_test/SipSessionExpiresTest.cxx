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

static const char* const SipUrlTest_cxx_Version =
    "$Id: SipSessionExpiresTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipSessionExpires.hxx"
#include "SipMinSE.hxx"
#include "Verify.hxx"
#include "InviteMsg.hxx"
#include "StatusMsg.hxx"
#include "SipSessionExpires.hxx"
#include "SipMinSE.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;

char* msg_invite = "INVITE sip:7013000@172.19.175.70:5060;user=phone;phone-context=unknown SIP/2.0\r\n\
Via: SIP/2.0/UDP 172.19.175.71:5060;branch=c419c2f0a1077bdf16b34df6384becc5.1\r\n\
Via: SIP/2.0/UDP 172.19.175.226:52901\r\n\
From: \"7010300\"<sip:7010300@172.19.175.226>\r\n\
To: <sip:7013000@172.19.175.71;user=phone;phone-context=unknown>\r\n\
Call-ID: 5CF43908-1A820032-0-4D8E29AC@172.19.175.226\r\n\
CSeq: 101 INVITE\r\n\
Proxy-Authorization: Basic VovidaClassXSwitch\r\n\
Max-Forwards: 5\r\n\
Expires: 60\r\n\
Session-Expires: 60;refresher=uas\r\n\
Min-SE: 60\r\n\
Record-Route: <sip:7013000@172.19.175.71:5060;maddr=172.19.175.71>\r\n\
Contact: <sip:7010300@172.19.175.226:5060;user=phone>\r\n\
Content-Length: 138\r\n\
Timestamp: 732245189\r\n\
User-Agent: Cisco VoIP Gateway/ IOS 12.x/ SIP enabled\r\n\
Content-Type: application/sdp\r\n\
\r\n\
v=0\r\n\
o=CiscoSystemsSIP-GW-UserAgent 6459 1977 IN IP4 172.19.175.226\r\n\
s=SIP Call\r\n\
c=IN IP4 172.19.175.226\r\n\
t=0 0\r\n\
m=audio 20752 RTP/AVP 0\r\n\
\r\n";


char* status_msg=    "SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.66.103:5060;branch=1\r\n\
Via: SIP/2.0/UDP 192.168.66.2:5060\r\n\
From: UserAgent<sip:5220@192.168.66.2:5060;user=phone>\r\n\
To: 5221<sip:5221@192.168.66.103:5060;user=phone>\r\n\
Call-ID: 209747329338013@192.168.66.2\r\n\
CSeq: 1 INVITE\r\n\
Record-Route: <sip:5221@192.168.66.103:5060>\r\n\
Session-Expires: 60;refresher=uas\r\n\
Min-SE: 60\r\n\
Content-Length: 0\r\n\
\r\n";



string data1("2000");
string data2("2000;refresher=uas");

void test()
{
    string host_addr = theSystem.gethostAddress();
    {
	// test a Session-Expires header
	SipSessionExpires testa(data1, host_addr);

	SipSessionExpires testb(testa);

	SipSessionExpires testc("", host_addr);

	testc = testa;

	test_verify(testa == testb);
	test_verify(testa == testc);
	test_verify(testc == testb);

	test_verify(testa.encode() == testb.encode());
	test_verify(testa.encode() == testc.encode());
	test_verify(testb.encode() == testc.encode());
	test_verify(testa.encode() == "Session-Expires: 2000\r\n");
    }

    {
	// test parameter

	SipSessionExpires test_00(data2, host_addr);
	SipSessionExpires test_01(string("2000"), host_addr);
        test_01.setRefreshParam("uas");

	test_verify(test_00 == test_01);
	test_verify(test_00.encode() == "Session-Expires: 2000;refresher=uas\r\n");
    }
    {
        InviteMsg msg(msg_invite);
        InviteMsg msg2("", host_addr);
        msg2 = msg;
        test_verify(msg2 == msg);
        test_verify(msg2.encode() == msg.encode());
        test_verify(msg2.getSessionExpires(host_addr).getDelta() == "60");
        test_verify(msg2.getMinSE(host_addr).getDelta() == "60");

        SipSessionExpires se("", host_addr);
        se.setDelta("20");
        msg2.setSessionExpires(se, host_addr);
        test_verify(msg2.getSessionExpires(host_addr).getDelta() == "20");
    }
    {
        SipMinSE test_02("20000", host_addr);
        SipMinSE test_03("", host_addr);
        test_03 = test_02;
        test_verify(test_03 == test_02);
        test_verify(test_03.encode()  == test_02.encode());
        test_verify(test_02.encode() == ("Min-SE: 20000\r\n"));
    }
    {
        StatusMsg sMsg(status_msg);
        StatusMsg sMsg2("", host_addr);
        sMsg2 = sMsg;
        test_verify(sMsg2 == sMsg);
        test_verify(sMsg2.encode() == sMsg.encode());
        test_verify(sMsg2.getSessionExpires().getDelta() == "60");
    }

}

int main()
{
    test();
    return test_return_code(20);
}
