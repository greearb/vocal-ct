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

static const char* const SipRawHeaderTest_cxx_Version =
    "$Id: SipRawHeaderTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "Verify.hxx"
#include "SipRawHeader.hxx"
#include "SipTo.hxx"
#include "SipContact.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


void test()
{
    {
	test_verify(headerTypeDecode("to") == SIP_TO_HDR);
	test_verify(headerTypeDecode("via") == SIP_VIA_HDR);
	test_verify(headerTypeDecode("from") == SIP_FROM_HDR);
	
	SipRawHeader x(host_addr);
	x.decode("To: \"Bryan Ogawa\" <sip:bko@cisco.com>\r\n");
	
	SipRawHeader y(x);
	
	test_verify(x == y);
	
	y.parse();
	test_verify(x == y);
	y.header = new SipTo("", host_addr); // reset
	test_verify(x != y);
    }

    {
	// test multiple headers
	SipRawHeader x(host_addr);
	x.decode("Contact: \"OGAWA, Bryan\" <sip:bko@cisco.com>, \"DOE, John\" <sip:jdoe@test.com>\r\n"); //

	test_verify(x.next != 0);

	SipContact empty("", host_addr);

	Sptr<SipContact> tmp;
	SipContact first("\"OGAWA, Bryan\" <sip:bko@cisco.com>", host_addr);
	x.parse();
	tmp.dynamicCast(x.header);
	test_verify(*tmp == first);
	test_verify(*tmp != empty);


	SipContact second("\"DOE, John\" <sip:jdoe@test.com>", host_addr);

	x.next->parse();
	tmp.dynamicCast( x.next->header);
	test_verify(*tmp == second);
	test_verify(*tmp != empty);

	test_verify(first != second);
    }
    {
	SipRawHeader test(host_addr);
	test.decode("Record-Route: <sip:4000@172.19.175.150:5060;maddr=172.19.175.150>,<sip:5452@172.19.175.150:5060;maddr=172.19.175.150>");
	test_verify(test.next != 0);
	Data encoding;
	test.encode(&encoding);
	test_verify(encoding == "Record-Route: <sip:4000@172.19.175.150:5060;maddr=172.19.175.150>,<sip:5452@172.19.175.150:5060;maddr=172.19.175.150>\r\n");
//	cout << encoding << endl;

	test_verify(test.headerValue == "<sip:4000@172.19.175.150:5060;maddr=172.19.175.150>");
    }

}

int main()
{
    test();
    return test_return_code(15);
}
