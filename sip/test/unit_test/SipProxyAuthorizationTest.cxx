/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2002-2003 Vovida Networks, Inc.  All rights reserved.
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

static const char* const SipProxyAuthorizationTest_cxx_Version =
    "$Id: SipProxyAuthorizationTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipProxyAuthorization.hxx"
#include "Verify.hxx"
#include "SipDigest.hxx"
#include "symbols.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();


using namespace Vocal;

void test()
{
    {
	SipProxyAuthorization test1("Digest username=\"4283044\",realm=\"172.19.174.209\",uri=\"sip:172.19.174.209\",response=\"714bc773da83adedb0346f9783bb06ca\",nonce=\"1013813709\",algorithm=MD5", host_addr);
	
	test_verify(test1.getTokenValue( "realm" ) == "172.19.174.209");
	test_verify(test1.getTokenValue( "username" ) == "4283044");
	test_verify(test1.getTokenValue( "uri" ) == "sip:172.19.174.209");
	test_verify(test1.getTokenValue( "algorithm" ) == "MD5");
cerr << "TEST:" << test1.encode().logData() << endl;
	test_verify(test1.encode() == "Proxy-Authorization: Digest algorithm=MD5,nonce=\"1013813709\",realm=\"172.19.174.209\",response=\"714bc773da83adedb0346f9783bb06ca\",uri=\"sip:172.19.174.209\",username=\"4283044\"\r\n");
    }
    {
	SipProxyAuthorization test1("Digest username=\"4283044\"  ,realm=\"172.19.174.209\", uri=\"sip:172.19.174.209\"  ,  response=\"714bc773da83adedb0346f9783bb06ca\",  nonce=\"1013813709\" ,algorithm=MD5   ", host_addr);
	
	test_verify(test1.getTokenValue( "realm" ) == "172.19.174.209");
	test_verify(test1.getTokenValue( "username" ) == "4283044");
	test_verify(test1.getTokenValue( "uri" ) == "sip:172.19.174.209");
	test_verify(test1.getTokenValue( "algorithm" ) == "MD5");
	test_verify(test1.encode() == "Proxy-Authorization: Digest algorithm=MD5,nonce=\"1013813709\",realm=\"172.19.174.209\",response=\"714bc773da83adedb0346f9783bb06ca\",uri=\"sip:172.19.174.209\",username=\"4283044\"\r\n");
    }
    {
	Sptr<SipProxyAuthorization> authorization = new SipProxyAuthorization("", host_addr);

	Data nonce = "1012951006";
	Data user = "";
	Data pwd = "password";
	Data method = "REGISTER";
	Data requestURI = "sip:172.19.174.209:5060";
	Data qop;
	Data cnonce;
	Data alg = "MD5";
	Data noncecount;
	Data opaque;
	Data realm = "172.19.174.209";

	SipDigest sipDigest;

	Data response = sipDigest.form_SIPdigest(nonce, user, pwd, method,
						 requestURI, realm, qop, 
						 cnonce, alg, noncecount);
	
	//set this as response in authorization.    
	authorization->setAuthScheme(AUTH_DIGEST);
	
	authorization->setTokenDetails("username", user); //1
	authorization->setTokenDetails("realm", realm);  //2
	authorization->setTokenDetails("nonce", nonce);  //3
	authorization->setTokenDetails("response", response); //4
	authorization->setTokenDetails("qop", qop); //5
	authorization->setTokenDetails("uri", requestURI); //6
	authorization->setTokenDetails("cnonce", cnonce); //7
	authorization->setTokenDetails("nc", noncecount); //8
	authorization->setTokenDetails("opaque", opaque); //9
	cout << authorization->encode();
    }
    {
	SipProxyAuthorization test1("Digest username=4283044,realm=172.19.174.209,uri=\"sip:172.19.174.209\",response=\"714bc773da83adedb0346f9783bb06ca\",nonce=\"1013813709\",algorithm=MD5", host_addr);
	
	test_verify(test1.getTokenValue( "realm" ) == "172.19.174.209");
	test_verify(test1.getTokenValue( "username" ) == "4283044");
	test_verify(test1.getTokenValue( "uri" ) == "sip:172.19.174.209");
	test_verify(test1.getTokenValue( "algorithm" ) == "MD5");
cerr << "TEST:" << test1.encode().logData() << endl;
	test_verify(test1.encode() == "Proxy-Authorization: Digest algorithm=MD5,nonce=\"1013813709\",realm=\"172.19.174.209\",response=\"714bc773da83adedb0346f9783bb06ca\",uri=\"sip:172.19.174.209\",username=\"4283044\"\r\n");
    }
    {
	SipProxyAuthorization test("Basic TestTestTestTest", host_addr);

	test_verify(test.getAuthScheme() == AUTH_BASIC);
	test_verify(test.getBasicCookie() == "TestTestTestTest");
	test_verify(test.encode() == "Proxy-Authorization: Basic TestTestTestTest\r\n");
    }
}


int main()
{
    test();
    return test_return_code(18);
}
