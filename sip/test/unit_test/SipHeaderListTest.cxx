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

static const char* const SipHeaderListTest_cxx_Version =
    "$Id: SipHeaderListTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "Verify.hxx"
#include "SipRawHeader.hxx"
#include "SipTo.hxx"
#include "SipContact.hxx"
#include "SipHeaderList.hxx"
#include "SipRecordRoute.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


char* testList[] = 
{
    "<sip:4000@172.19.175.150:5060;maddr=172.19.175.150>",
    "<sip:5452@172.19.175.150:5060;maddr=172.19.175.150>",
    "<sip:6000@172.19.175.150:5060;maddr=172.19.175.150>",
    "<sip:7000@172.19.175.150:5060;maddr=172.19.175.150>",
    "<sip:8000@172.19.175.150:5060;maddr=172.19.175.150>",
    0
};

void test()
{
    {
	Sptr<SipRawHeader> test = new SipRawHeader(host_addr);
	Data msg = "Record-Route: ";
	bool first = true;
	for(char** p = testList; (*p != 0); p++)
	{
	    if(!first)
	    {
		msg += ",";
	    }
	    msg += *p;
	    first = false;
	}
	test->decode(msg);

	SipHeaderList<SipRecordRoute> list(test);
	SipHeaderList<SipRecordRoute>::iterator i = list.begin();

	Sptr<SipRawHeader> tmp = test;
	int j = 0;
	while(tmp != 0)
	{
//	    cout << j << ": " << tmp->headerValue << endl;
	    j++;
	    tmp = tmp->next;
	}
	
//	cout << j << endl;
	test_verify(j == 5);

	j = 0;

	test_verify(i != list.end());
	for(i = list.begin(); i != list.end(); i++)
	{
	    j++;
	}
//	cout << j << endl;
	test_verify(j == 5);

	j = 0;
	for(i = list.begin(); i != list.end(); i++)
	{
//	    cout << (*i)->encode() << endl;
	    test_verify((*i)->encode() == testList[j]);
	    ++j;
	}

	i++; i++; i++; // verify that you can run off the end safely
	test_verify(i == list.end());

    }

}

int main()
{
    test();
    return test_return_code(9);
}
