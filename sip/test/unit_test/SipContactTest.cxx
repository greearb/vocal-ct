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

static const char* const SipContactTest_cxx_Version =
    "$Id: SipContactTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipContact.hxx"
#include "Verify.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();


using namespace Vocal;

void test()
{
    {
	SipContact contact("tel:4443322", host_addr);
	cout << contact.encode();
	test_verify(contact.encode() == "Contact: <tel:4443322>\r\n");

	contact.setDisplayName("Bob Dole");
	test_verify(contact.encode() == "Contact: Bob Dole<tel:4443322>\r\n");
    }
    {
	SipContact contact("\"Quoted string \\\"\\\"\" <sip:jdrosen@bell-labs.com>;q=0.33;action=proxy;expires=3200", host_addr);
	test_verify(contact.getQValue() == "0.33");
	test_verify(contact.getExpires() == SipExpires("3200", host_addr));
	test_verify(contact.getExpires() != SipExpires("1", host_addr));
//	test_verify(contact.getAction() == "proxy");
//	cout << "a: " << contact.getAction();
	cout << "q: " << contact.getQValue();
	cout << contact.encode();
    }
    {
	SipContact contact("\"Quoted string \\\"\\\"\" <sip:jdrosen@bell-labs.com>;q=0.33;action=proxy;expire=3200", host_addr);
	SipContact contact2(contact);
	SipContact contact3("", host_addr);
	test_verify(contact == contact2);
	test_verify(contact.encode() == contact2.encode());

	test_verify(contact != contact3);
	test_verify(contact.encode() != contact3.encode());

	contact3 = contact;
	test_verify(contact == contact3);
	test_verify(contact.encode() == contact3.encode());

    }
    {
	SipContact contact("<sip:0000013@192.168.2.206:5060;transport=udp>;action=proxy;expires=36000", host_addr);
	test_verify(contact.getExpires() == SipExpires("36000", host_addr));
	test_verify(contact.encode() == "Contact: <sip:0000013@192.168.2.206:5060;transport=udp>;action=proxy;expires=36000\r\n");
	cout << contact.encode();
    }
    {
      SipContact contact ("<sip:2046@172.23.252.247:5060;user=phone>", host_addr);
    }
}

int main()
{
    test();
    return test_return_code(13);
}
