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

static const char* const SipTcpFundamentalsTest_cxx_Version =
    "$Id: SipTcpFundamentalsTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "Verify.hxx"
#include "SipTcpConnection.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


char* msgtext[] = { 
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
\r\n",
// this is the second message
"INVITE sip:7013000@172.19.175.70:5060;user=phone;phone-context=unknown SIP/2.0\r\n\
Via: SIP/2.0/UDP 172.19.175.71:5060;branch=c419c2f0a1077bdf16b34df6384becc5.1\r\n\
Via: SIP/2.0/UDP 172.19.175.226:52901\r\n\
From: \"7010300\"<sip:7010300@172.19.175.226>\r\n\
To: <sip:7013000@172.19.175.71;user=phone;phone-context=unknown>\r\n\
Call-ID: 5CF43908-1A820032-0-4D8E29AC@172.19.175.226\r\n\
CSeq: 101 INVITE\r\n\
Proxy-Authorization: Basic VovidaClassXSwitch\r\n\
Max-Forwards: 5\r\n\
Expires: 60\r\n\
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
\r\n",
"INVITE sip:vivekg@chair.dnrc.bell-labs.com SIP/2.0
TO :
 sip:vivekg@chair.dnrc.bell-labs.com ;   tag    = 1a1b1f1H33n
From   : \"J Rosenberg \\\\\\\"\" <sip:jdrosen@lucent.com>
  ;
  tag = 98asjd8
CaLl-Id
 : 0ha0isndaksdj@10.1.1.1
cseq: 8
  INVITE
Via  : SIP  /   2.0 
 /UDP 
    135.180.130.133
Subject :
NewFangledHeader:   newfangled value
 more newfangled value
Content-Type: application/sdp
v:  SIP  / 2.0  / TCP     12.3.4.5   ;
  branch  =   9ikj8  ,
 SIP  /    2.0   / UDP  1.2.3.4   ; hidden   
m:\"Quoted string \\\"\\\"\" <sip:jdrosen@bell-labs.com> ; newparam = newvalue ;
  secondparam = secondvalue  ; q = 0.33
  ,
 tel:4443322

v=0
o=mhandley 29739 7272939 IN IP4 126.5.4.3
s= 
c=IN IP4 135.180.130.88
m=audio 49210 RTP/AVP 0 12
m=video 3227 RTP/AVP 31
a=rtpmap:31 LPC/8000

",
// this is third message
        "INVITE sip:7013000@172.19.175.70:5060;user=phone;phone-context=unknown SIP/2.0\r\n",
        "INVITE sip:7013000@172.19.175.70:5060;user=phone;phone-context=unknown SIP/2.0\n", // 4
        "\r\n\r\n", // 5
        "\r\n\r\nINVITE sip:7013000@172.19.175.70:5060;user=phone;phone-context=unknown SIP/2.0\n\r\n", // 6
        "\r\n\r\nINVITE sip:7013000@172.19.175.70:5060;user=phone;phone-context=unknown SIP/2.0\nContent-Length: 0\r\n\r\n", // 7
        "\r\n\r\nINVITE sip:7013000@172.19.175.70:5060;user=phone;phone-context=unknown SIP/2.0\nContent-Length: 10\r\n\r\n", // 8
        "\r\n\r\n\r\n\r\n", // 9
        "\n\n\n\n", // 10
};    


void test()
{
    test_verify(isFullMsg("") == -1);
    test_verify(isFullMsg("\r") == -1);
    test_verify(isFullMsg("\r\n") == -1);
    test_verify(isFullMsg(msgtext[0]) > -1);
    test_verify(isFullMsg(msgtext[1]) > -1);
    test_verify(isFullMsg(msgtext[2]) == -1);
    test_verify(isFullMsg(msgtext[3]) == -1);
    test_verify(isFullMsg(msgtext[4]) == -1);
    test_verify(isFullMsg(msgtext[5]) == -1);
    test_verify(isFullMsg(msgtext[6]) > -1);
    test_verify(isFullMsg(msgtext[7]) > -1);
    test_verify(isFullMsg(msgtext[8]) == -1);
    test_verify(isFullMsg(msgtext[9]) == -1);
    test_verify(isFullMsg(msgtext[10]) == -1);
}


int main()
{
    test();
    return test_return_code(14);
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
