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

static const char* const SipDigestTest_cxx_Version =
    "$Id: SipDigestTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipDigest.hxx"
#include "Verify.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


void test1()
{
    Data nonce("nonce");
    Data user("sunitha");
    Data passwd("passwd");
    Data method("INVITE");
    Data uri("sip:vovida.com");
    Data realm("vovida.com");
    Data qop("12");
    Data cnonce("12345");
    Data alg("MD5");
    Data noncecount("12345");

    Data nonce2("nonce");
    Data user2("sunitha");
    Data passwd2("passwd");
    Data method2("INVITE");
    Data uri2("sip:vovida.com");
    Data realm2("vovida.com");
    Data qop2("12");
    Data cnonce2("12345");
    Data alg2("MD5");
    Data noncecount2("12345");
    


    SipDigest first;
    SipDigest second;

    Data firstSum = first.form_SIPdigest(nonce, user, passwd, method, uri,
	                                 realm, qop, cnonce, alg, noncecount);
    Data secondSum = second.form_SIPdigest(nonce2, user2, passwd2, method2, 
					   uri2, realm2, qop2, cnonce2, alg2,
					   noncecount2);
    test_verify(firstSum == secondSum);
}


void test2()
{
//    Proxy-Authorization: Digest username="1000",realm="172.19.174.209",uri="sip:172.19.174.209",response="ff71894dadde9d023458aa19ee421247",nonce="1012970712",algorithm=MD5
    Data username("1000");
    Data passwd("password");
    Data method("REGISTER");
    Data uri("sip:172.19.174.209");
    Data realm("172.19.174.209");
    Data qop("");
    Data nonce("1012970712");
    Data cnonce("");
    Data algorithm("MD5");
    Data noncecount("");

    SipDigest first;
    Data firstSum = first.form_SIPdigest(nonce, username, passwd, method, uri,
	                                 realm, qop, cnonce, algorithm, 
					 noncecount);

    test_verify(firstSum == "ff71894dadde9d023458aa19ee421247");
    cout << firstSum << endl;
}

int main()
{
    test1();
    test2();

    return test_return_code(2);
}
