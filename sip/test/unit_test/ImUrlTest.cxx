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
    "$Id: ImUrlTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "ImUrl.hxx"
#include "Verify.hxx"
#include "MessageMsg.hxx"
#include "SystemInfo.hxx"


using namespace Vocal;

char* msg_msg ="MESSAGE im:user2@domain.com SIP/2.0\r\n\
Via: SIP/2.0/UDP user1pc.domain.com\r\n\
From: im:user1@domain.com\r\n\
To: User2<im:user2@domain.com>\r\n\
Call-ID: asd88asd77a@1.2.3.4\r\n\
CSeq: 1 MESSAGE\r\n\
Content-Type: text/plain\r\n\
Content-Length: 18\r\n\
\r\n\
Watson, come here.\r\n\
\r\n";
 

string data1("im:user1@cisco.com");

void test()
{
    string host_addr = theSystem.gethostAddress();

//    cpLogSetPriority(LOG_DEBUG_STACK);
    {
	// test ImUrl
	ImUrl testa(data1);

	ImUrl testb(testa);

	ImUrl testc;

	testc = testa;

	test_verify(testa == testb);
	test_verify(testa == testc);
	test_verify(testc == testb);

	test_verify(testa.encode() == testb.encode());
	test_verify(testa.encode() == testc.encode());
	test_verify(testb.encode() == testc.encode());
	test_verify(testa.encode() == data1.c_str());
    }

    {
        MessageMsg msg(msg_msg);
        MessageMsg msg2("");
        msg2 = msg;
        test_verify(msg2 == msg);
        test_verify(msg2.encode() == msg.encode());
        Sptr<BaseUrl> bUrl = msg.getTo().getUrl();
        Sptr<ImUrl> imUrl;
        imUrl.dynamicCast(bUrl);
        test_verify(imUrl->getUserValue() == "user2");
        test_verify(imUrl->getHost() == "domain.com");
        test_verify(imUrl->encode() == "im:user2@domain.com");
    }
}

int main()
{
    test();
    return test_return_code(12);
}
