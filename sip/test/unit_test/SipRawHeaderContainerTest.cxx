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

static const char* const SipRawHeaderContainerTest_cxx_Version =
    "$Id: SipRawHeaderContainerTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "Verify.hxx"
#include "SipRawHeaderContainer.hxx"
#include "support.hxx"
#include "SipTo.hxx"
#include "SipVia.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


const char* msgtext = 
"Accept: application/sdp\r\n\
Call-ID: c4943000-75748-7058-2e323731@172.19.174.170\r\n\
Contact: <sip:5451@172.19.174.170:5060>\r\n\
Content-Length: 228\r\n\
Content-Type: application/sdp\r\n\
CSeq: 101 INVITE\r\n\
Expires: 180\r\n\
From: \"5451\"<sip:5451@172.19.175.150>\r\n\
Record-Route: <sip:5452@172.19.175.150:5060;maddr=172.19.175.150>,<sip:5452@172.19.175.150:5060;maddr=172.19.175.150>\r\n\
To: <sip:5452@172.19.175.150>\r\n\
User-Agent: Cisco IP Phone/ Rev. 1/ SIP enabled\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.4\r\n\
Via: SIP/2.0/UDP 172.19.175.150:5060;branch=d1faef3fff22fbedaa794b8ea2abc7e5.2\r\n\
Via: SIP/2.0/UDP 172.19.174.170:5060\r\n\
";



void test()
{
    {
	SipRawHeaderContainer container;
	
	for(int i = 0; i < 25; i++)
	{
	    Sptr<SipRawHeader> p = new SipRawHeader(host_addr);
	    
	    p->headerType = SIP_VIA_HDR;
	    p->headerValue = itos(i);
	    container.insertHeader(p);
	}
	test_verify(container.getHeader(SIP_VIA_HDR) != 0);
	test_verify(container.getHeader(SIP_VIA_HDR)->next != 0);
	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 25);

	container.setNumHeaders(SIP_VIA_HDR, 0);
	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 0);
    }


    {
	SipRawHeaderContainer container;
	
	for(int i = 0; i < 6; i++)
	{
	    Sptr<SipRawHeader> p = new SipRawHeader(host_addr);
	    
	    p->headerType = SIP_VIA_HDR;
	    p->headerValue = itos(i);
	    container.insertHeader(p);
	}

	test_verify(container.getHeader(SIP_VIA_HDR, 0)->headerValue == "0");
	test_verify(container.getHeader(SIP_VIA_HDR, 1)->headerValue == "1");
	test_verify(container.getHeader(SIP_VIA_HDR, 2)->headerValue == "2");
	test_verify(container.getHeader(SIP_VIA_HDR, 3)->headerValue == "3");
	test_verify(container.getHeader(SIP_VIA_HDR, 4)->headerValue == "4");
	test_verify(container.getHeader(SIP_VIA_HDR, 5)->headerValue == "5");

	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 6);
	container.removeHeader(SIP_VIA_HDR);
	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 5);

	test_verify(container.getHeader(SIP_VIA_HDR, 0)->headerValue == "0");
	test_verify(container.getHeader(SIP_VIA_HDR, 1)->headerValue == "1");
	test_verify(container.getHeader(SIP_VIA_HDR, 2)->headerValue == "2");
	test_verify(container.getHeader(SIP_VIA_HDR, 3)->headerValue == "3");
	test_verify(container.getHeader(SIP_VIA_HDR, 4)->headerValue == "4");

	container.removeHeader(SIP_VIA_HDR, 1);
	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 4);
	test_verify(container.getHeader(SIP_VIA_HDR, 0)->headerValue == "0");
	test_verify(container.getHeader(SIP_VIA_HDR, 1)->headerValue == "2");
	test_verify(container.getHeader(SIP_VIA_HDR, 2)->headerValue == "3");
	test_verify(container.getHeader(SIP_VIA_HDR, 3)->headerValue == "4");

	container.removeHeader(SIP_VIA_HDR, 0);
	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 3);
	test_verify(container.getHeader(SIP_VIA_HDR, 0)->headerValue == "2");
	test_verify(container.getHeader(SIP_VIA_HDR, 1)->headerValue == "3");
	test_verify(container.getHeader(SIP_VIA_HDR, 2)->headerValue == "4");


	container.removeHeader(SIP_VIA_HDR, 2);
	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 2);
	test_verify(container.getHeader(SIP_VIA_HDR, 0)->headerValue == "2");
	test_verify(container.getHeader(SIP_VIA_HDR, 1)->headerValue == "3");

	container.setNumHeaders(SIP_VIA_HDR, 0);
	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 0);

	Sptr<SipVia> via1 = new SipVia("", host_addr);
	via1->setHost("192.168.5.1");
	test_verify(via1->getHost() == "192.168.5.1");
	container.appendHeader(SIP_VIA_HDR, via1, host_addr);
	test_verify(getParsed<SipVia>(container, SIP_VIA_HDR, host_addr, 0).getHost() == "192.168.5.1");
	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 1);

	Sptr<SipVia> tmp;
	container.getParsedHeader(tmp, SIP_VIA_HDR, 0);
	test_verify(tmp->getHost() == "192.168.5.1");

	Sptr<SipVia> via2 = new SipVia("", host_addr);
	via2->setHost("192.168.5.2");
	container.appendHeader(SIP_VIA_HDR, via2, 0);
	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 2);
	test_verify(getParsed<SipVia>(container, SIP_VIA_HDR, host_addr, 0).getHost() == "192.168.5.2");
	test_verify(getParsed<SipVia>(container, SIP_VIA_HDR, host_addr, 1).getHost() == "192.168.5.1");


	Sptr<SipVia> via3 = new SipVia("", host_addr);
	via3->setHost("192.168.5.3");
	container.appendHeader(SIP_VIA_HDR, via3, 0);
	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 3);
	test_verify(getParsed<SipVia>(container, SIP_VIA_HDR, host_addr, 0).getHost() == "192.168.5.3");
	test_verify(getParsed<SipVia>(container, SIP_VIA_HDR, host_addr, 1).getHost() == "192.168.5.2");
	test_verify(getParsed<SipVia>(container, SIP_VIA_HDR, host_addr, 2).getHost() == "192.168.5.1");


	Sptr<SipVia> via4 = new SipVia("", host_addr);
	via4->setHost("192.168.5.4");
	container.appendHeader(SIP_VIA_HDR, via4, host_addr, 1);
	test_verify(container.getNumHeaders(SIP_VIA_HDR) == 4);

	test_verify(getParsed<SipVia>(container, SIP_VIA_HDR, host_addr, 0).getHost() == "192.168.5.3");
	test_verify(getParsed<SipVia>(container, SIP_VIA_HDR, host_addr, 1).getHost() == "192.168.5.4");
	test_verify(getParsed<SipVia>(container, SIP_VIA_HDR, host_addr, 2).getHost() == "192.168.5.2");
	test_verify(getParsed<SipVia>(container, SIP_VIA_HDR, host_addr, 3).getHost() == "192.168.5.1");


    }


    // test parsing
    {
	SipRawHeaderContainer container;
	Data data(msgtext);
	container.decode(&data, host_addr);
	test_verify(container.getHeader(SIP_VIA_HDR) != 0);
	test_verify(container.getHeader(SIP_FROM_HDR) != 0);
	test_verify(container.getHeader(SIP_TO_HDR) != 0);
	test_verify(container.getHeader(SIP_CSEQ_HDR) != 0);
	test_verify(container.getHeader(SIP_CALLID_HDR) != 0);
	test_verify(container.getHeader(SIP_DATE_HDR) == 0);
	test_verify(container.getHeader(SIP_RECORD_ROUTE_HDR) != 0);
	test_verify(container.getHeader(SIP_RECORD_ROUTE_HDR)->next != 0);



	Data encodedData;
	container.encode(&encodedData);
        
	SipRawHeaderContainer copy;

	copy = container;

	Data encodedData2;
	copy.encode(&encodedData2);
	test_verify(encodedData2 == encodedData); // xxx broken

	// now, test the To line for proper construction

	Sptr<SipTo> myTo;
	copy.getParsedHeader(myTo, SIP_TO_HDR, 0);

	test_verify(myTo != 0);

	test_verify(myTo->getUser() == "5452");

	test_verify(copy.getNumHeaders(SIP_VIA_HDR) == 3);
	test_verify(copy.getNumHeaders(SIP_TO_HDR) == 1);

	Sptr<SipVia> myVia;

	copy.getParsedHeader(myVia, SIP_VIA_HDR, host_addr, -1);

	test_verify(myVia->getHost() == "172.19.174.170");

    }

    /// test some insertion semantics?
    {
	SipRawHeaderContainer container;
	Sptr<SipTo> to;
	container.getParsedHeader(to, SIP_TO_HDR, host_addr);

	SipTo empty("", host_addr);

cerr << "TO1:" << to->encode().logData() << endl;
cerr << "TO2:" << empty.encode().logData() << endl;

	test_verify(*to == empty);

	to->setHost("192.168.5.3");

	Sptr<SipTo> x;
	container.getParsedHeader(x, SIP_TO_HDR, host_addr);

	test_verify(*to == *x);
	test_verify(x->getHost() == "192.168.5.3");

	SipTo newTo("", host_addr);

//	newTo.setHost("127.0.0.1");
	newTo.setUser("foo");

	Sptr<SipTo> y;
	container.getParsedHeader(y, SIP_TO_HDR, host_addr);
	*y = newTo;

	Sptr<SipTo> z;
	container.getParsedHeader(z, SIP_TO_HDR, host_addr);

	test_verify(*z == newTo); // xxx broken
	cout << (*z).encode() << endl;
	cout << newTo.encode() << endl;
    }
    {
	SipRawHeaderContainer container1;
	SipRawHeaderContainer container2;
	Data data(msgtext);
	container1.decode(&data, host_addr);
	data = msgtext;
	container2.decode(&data, host_addr);



	test_verify(container1 == container2);
	test_verify(container2 == container1);
	SipTo newTo("", host_addr);
	Sptr<SipTo> y;
	container1.getParsedHeader(y, SIP_TO_HDR, host_addr);
	*y = newTo;
	test_verify(container1 != container2);
	test_verify(container2 != container1);
    }

}

int main()
{
    test();
    return test_return_code(68);
}
