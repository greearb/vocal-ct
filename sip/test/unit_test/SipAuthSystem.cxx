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

static const char* const SipAuthSystem_cxx_Version =
    "$Id: SipAuthSystem.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "Verify.hxx"
#include "RegisterMsg.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


// sample message -- valid

char* register_txt1 = "REGISTER sip:172.19.174.209 SIP/2.0\r
Via: SIP/2.0/UDP 10.22.76.105:5060\r
From: sip:1000@172.19.174.209\r
To: sip:1000@172.19.174.209\r
Call-ID: 003094c2-e6a72210-60809eeb-063d3755@10.22.76.105\r
Date: Fri, 08 Feb 2002 21:21:08 GMT\r
CSeq: 102 REGISTER\r
User-Agent: Cisco-SIP-IP-Phone/3\r
Contact: sip:1000@10.22.76.105:5060\r
Proxy-Authorization: Digest username=\"1000\",realm=\"172.19.174.209\",uri=\"sip:172.19.174.209\",response=\"ff71894dadde9d023458aa19ee421247\",nonce=\"1012970712\",algorithm=MD5\r
Content-Length: 0\r
Expires: 3600\r
\r
";
char* register_txt2 =
"REGISTER sip:172.19.174.209 SIP/2.0\r
Via: SIP/2.0/UDP 10.22.76.105:5060\r
From: sip:1000@172.19.174.209\r
To: sip:1000@172.19.174.209\r
Call-ID: 003094c2-e6a7236f-167720c0-03bf19a2@10.22.76.105\r
Date: Sat, 09 Feb 2002 01:47:36 GMT\r
CSeq: 102 REGISTER\r
User-Agent: Cisco-SIP-IP-Phone/3\r
Contact: sip:1000@10.22.76.105:5060\r
Proxy-Authorization: Digest username=\"1000\",realm=\"172.19.174.209\",uri=\"sip:172.19.174.209\",response=\"43bb79a8f0fd4599d357dd2d0f6b9c52\",nonce=\"1013141208\",algorithm=MD5\r
Content-Length: 0\r
Expires: 3600\r
\r
"
;

//    Proxy-Authorization: Digest username="1000",realm="172.19.174.209",uri="sip:172.19.174.209",response="ff71894dadde9d023458aa19ee421247",nonce="1012970712",algorithm=MD5


void test1()
{
    RegisterMsg regMsg1(register_txt1);

    test_verify(regMsg1.checkAuthDigest("1012970712", 
				       "1000", 
				       "password", 
				       "172.19.174.209", 
				       "sip:172.19.174.209") == true);

    RegisterMsg regMsg2(register_txt2);

    test_verify(regMsg2.checkAuthDigest("1013141208", 
				       "1000", 
				       "password", 
				       "172.19.174.209", 
				       "sip:172.19.174.209") == true);
}

int main()
{
    cpLogSetPriority(LOG_DEBUG_STACK);
    test1();
    return test_return_code(2);
}
