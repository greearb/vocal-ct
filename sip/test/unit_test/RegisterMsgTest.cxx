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

static const char* const RegisterMsgTest_cxx_Version =
    "$Id: RegisterMsgTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipAuthorization.hxx"
#include <cassert>
#include "cpLog.h"
#include "RegisterMsg.hxx"
#include "Verify.hxx"
#include "Sptr.hxx"
#include "BaseUrl.hxx"
#include "SipUser.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;
    
//message sent by telecaster
char* msgtext1[] = { 
"REGISTER sip:192.168.2.208:5061 SIP/2.0\r\n\
Via: SIP/2.0/UDP 192.168.2.205:7777\r\n\
From: <sip:000013@192.168.2.206:5060;user=phone>\r\n\
To: <sip:000013@192.168.2.206:5060;user=phone>\r\n\
Call-ID: 000013@192.168.2.206:5060\r\n\
CSeq: 2 REGISTER\r\n\
Contact: <sip:0000013@192.168.2.206:5060;transport=udp>;action=proxy;expires=36000\r\n\
User-Agent: Cisco ATA182 v.1.44\r\n\
Content-Length: 0\r\n\r\n",
};    
    
void testEncodeDecode()
{
    string host_addr = theSystem.gethostAddress();

    {
	Sptr <RegisterMsg> sipRegister;
	sipRegister = new RegisterMsg (msgtext1[0], host_addr);
	Data d1 = sipRegister->encode();
	cerr << d1 << endl;
	cerr << "--------------------------------------------------\n";
	SipContact c = sipRegister->getContact(0);
	SipContact d = sipRegister->getContact(0);
	Data d2 = sipRegister->encode();
	cerr << d2 << endl;
	test_verify(d1 == d2);
	test_verify(d.getExpires() == SipExpires(Data(36000), host_addr));
//	test_verify(d.getAction() == Data("proxy"));  // action no longer applies to Contact
    }
}


#include "RegisterMsg.hxx"

void
testJason()
{
    string host_addr = theSystem.gethostAddress();
    Sptr <RegisterMsg> sipRegister;
    sipRegister = new RegisterMsg ("REGISTER sip:192.168.2.208:5061 SIP/2.0\r\n" 
				   "Via: SIP/2.0/UDP 192.168.2.205:7777\r\n"
				   "From: <sip:000013@192.168.2.206:5060;user=phone>\r\n"
				   "To: <sip:000013@192.168.2.206:5060;user=phone>\r\n"
				   "Call-ID: 000013@192.168.2.206:5060\r\n"
				   "CSeq: 2 REGISTER\r\n"
				   "Contact: <sip:0000013@192.168.2.206:5060;transport=udp>;action=proxy;expires=36000\r\n"  
				   "User-Agent: Cisco ATA182 v.1.44\r\n"
				   "Content-Length: 0\r\n\r\n", host_addr);
    
    Data d1 = sipRegister->encode();
    cerr << d1 << endl;
    SipContact c = sipRegister->getContact(0);
    Data d2 = sipRegister->encode();
    cerr << d2 << endl;
    test_verify(d1 == d2);
    
    test_verify(sipRegister->getRequestLine().getUrl()->encode() == "sip:192.168.2.208:5061");
}


void
testJason2()
{
    string host_addr = theSystem.gethostAddress();
    Sptr <RegisterMsg> sipRegister;
    sipRegister = new RegisterMsg ("REGISTER sip:192.168.2.208 SIP/2.0\r\n" 
				   "Via: SIP/2.0/UDP 192.168.2.205:7777\r\n"
				   "From: <sip:000013@192.168.2.206:5060;user=phone>\r\n"
				   "To: <sip:000013@192.168.2.206:5060;user=phone>\r\n"
				   "Call-ID: 000013@192.168.2.206:5060\r\n"
				   "CSeq: 2 REGISTER\r\n"
				   "Contact: <sip:0000013@192.168.2.206:5060;transport=udp>;action=proxy;expires=36000\r\n"  
				   "User-Agent: Cisco ATA182 v.1.44\r\n"
				   "Content-Length: 0\r\n\r\n", host_addr);

   test_verify(sipRegister->getRequestLine().getUrl()->encode() == "sip:192.168.2.208");
}




int main()
{
    testEncodeDecode();
    testJason();

    return test_return_code(4);
}
