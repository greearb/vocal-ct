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
    "$Id: SipUrlTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipUrl.hxx"
#include "Verify.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


void test()
{
    {
	// test a simple SipUrl


	SipUrl testa(string("sip:2000@192.168.5.111:5061"), host_addr);

	SipUrl testb(testa);

	SipUrl testc("", host_addr);

	testc = testa;

	test_verify(testa == testb);
	test_verify(testa == testc);
	test_verify(testc == testb);

	test_verify(testa.encode() == testb.encode());
	test_verify(testa.encode() == testc.encode());
	test_verify(testb.encode() == testc.encode());

	cout << testa.encode() << endl;

	test_verify(testa.encode() == "sip:2000@192.168.5.111:5061");
    }

    {
	// test default values for port and transport

	SipUrl test_00(string("sip:2000@192.168.5.111"), host_addr);
	SipUrl test_01(string("sip:2000@192.168.5.111:5060"), host_addr);
	
	test_verify(test_00 == test_01);
	
	SipUrl test_02("", host_addr);
	
	test_verify(test_00 != test_02);
	
	SipUrl test_03(string("sip:2000@192.168.5.111;transport=udp"), host_addr);
	SipUrl test_04(string("sip:2000@192.168.5.111"), host_addr);
	
	test_verify(test_03 != test_02);
	test_verify(test_03 == test_04);
    }

    {
	// test minimal constructions for equality
	SipUrl test_00("", host_addr);
	SipUrl test_01("", host_addr);

	test_00.setUserValue("foo");
	test_01.setUserValue("foo");
	test_verify(test_00 == test_01); // this fails due to hostMatch failing

	test_00.setHost("test.com");
	test_01.setHost("test.com");
	test_verify(test_00 == test_01);

    }
    {
	// test phone-context
	//SipUrl test0(string("sip:2000@192.168.5.111:5060;transport=udp;user=phone;phone-context=unknown"));

	//test_verify(test0.encode() == "sip:2000@192.168.5.111:5060;transport=udp;user=phone;phone-context=unknown");
    }
    {
	// test phone-context
	SipUrl test0(string("sip:7311000@192.168.5.111:5060;transport=udp;user=phone;phone-context=unknown"), host_addr);

//	test0.setGlobalPhoneNumber("7311000");

	test_verify(test0.encode() == "sip:7311000@192.168.5.111:5060;transport=udp;user=phone;phone-context=unknown");
	cout << test0.encode() << endl;
    }
    {
	SipUrl test0(string("sip:7321000@192.168.5.111:5060;user=phone"), host_addr);

//	test0.setGlobalPhoneNumber("7311000");

	test_verify(test0.encode() == "sip:7321000@192.168.5.111:5060;user=phone");
	cout << test0.encode() << endl;
    }
    {
	Data url1("sip:a.g.bell@bell-telephone.com:5060");
    
	SipUrl sipUrl(url1, host_addr);

	test_verify(sipUrl.encode() == url1);
	test_verify(sipUrl.getHost() == "bell-telephone.com");
	test_verify(sipUrl.getPort() == "5060");
    }
    {
	Data url2("sip:veer@vovida.com:5560;transport=udp?veeru%3D23");
	SipUrl sipUrl2(url2, host_addr);

	cout << sipUrl2.encode() << endl;
	test_verify(url2 == sipUrl2.encode());
	test_verify(sipUrl2.getHost() == "vovida.com");
	test_verify(sipUrl2.getPort() == "5560");
	test_verify(sipUrl2.getTransportParam() == "udp");
	cout << "transport param is:" << sipUrl2.getTransportParam().logData() << endl;

	cout << "other name is:" << sipUrl2.getOtherName().logData() << endl;
	cout << "other param is:" << sipUrl2.getOtherParam().logData() << endl;


	Data url3( "sip:12321321;isub=2142114;postd=2134*#-.2ppwww364@rock.com:5060;user=phone?header%3Dwe");
	    
	SipUrl sipUrl3(url3, host_addr);

	cerr << "ENCODE2:" << url3.logData() << endl;
	cerr << "ENCODE:" << sipUrl3.encode().logData() << endl;
	test_bug(sipUrl3.encode() == url3);
	cout << sipUrl3.encode() << endl;
	    
	    
	Data url4("sip:+23229292@vovida.com;tag=2344");
	SipUrl sipUrl4(url4, host_addr);
	test_verify(sipUrl4.encode() == url4);
	cout << sipUrl4.encode() << endl;

	Data fg = "sip:12321321;isub=2142114;postd=2134*#-.2ppwww364@rock.com:5060;user=phone?header=we";
	//Data fg = "sip:stones@vovida.com;ttl=25?subject=project%20x&prirty=urgent";
	//Data fg= "sip:veer@vovida.com:5060;transport=udp;user=ip;method=top?dg=flash";//;god";
	//	Data jk="sip:veer@vovida.com:5560;transport=udp?veeru=23";
	//Data hj = "Krishan Veer<sip:veer@vovida.com>;tag=111111";
	Data hj = " KRISHAN VEER<sip:veer@vovida.com>;tag=111111;mthod=777";
	//  Data hj = "Krishan Veer<sip:veer@vovida.com ";
	//    Data hj = "sip:+23229292@vovida.com;tag=23444";
	//Data hj = "Krishan Veer sip:veer@vovida.com>";
	Data hj1 = "dsfagah-afdsggh-fadsgh@vovida.com";
    }
    {
      try
	{
	  SipUrl url1(Data("user1@host"), host_addr);
	  test_verify(false);
	}
      catch(...)
	{
	  test_verify(true);
	}
    }

    {
	SipUrl url1(Data("sip:user1@host"), host_addr);
	SipUrl url2(Data("sip:user2@host"), host_addr);
	SipUrl url3(Data("sip:user1@host"), host_addr);

	test_verify(url1 == url3);
	test_verify(url1 != url2);

        SipUrl url4(Data("sip:user:password@128.107.105.124:5060;"), host_addr);
        test_verify(url4.getHost() == Data("128.107.105.124"));
        test_verify(url4.getUserValue() == Data("user:password"));


        SipUrl url5(Data("sip:user%xyz:5061@host:5060"), host_addr);
        test_verify(url5.getUserValue() == Data("user%xyz:5061"));
        test_verify(url5.getHost() == Data("host"));

        //IPV6 test cases, a convertion from IPV4 address 3e.e0.39.92 (HEX)
        SipUrl url6(Data("sip:user@[2002:3ee0:3972:0001::1]:5060;"), host_addr);
        test_verify(url6.getUserValue() == Data("user"));
        test_verify(url6.getHost() == Data("[2002:3ee0:3972:0001::1]"));
	test_verify(url6.encode() == "sip:user@[2002:3ee0:3972:0001::1]:5060");

        SipUrl url7(Data("sip:[2002:3ee0:3972:0001::1]:5060;"), host_addr);
        test_verify(url7.getHost() == Data("[2002:3ee0:3972:0001::1]"));

        SipUrl url8(Data("sip:[2201:056D::112E:144A:1E24];"), host_addr);
        test_verify(url8.getHost() == Data("[2201:056D::112E:144A:1E24]"));

        SipUrl url9(Data("sip:1001@[2201:056D::112E:144A:1E24];transport=tcp;"), host_addr);
        test_verify(url9.getUserValue() == Data("1001"));
        test_verify(url9.getHost() == Data("[2201:056D::112E:144A:1E24]"));

        SipUrl url10(url8);
        SipUrl url11("", host_addr);
        url11 = url8;
        test_verify(url10.encode() == url8.encode());
        test_verify(url11.encode() == url8.encode());

    }

    {
	SipUrl myUrl("", host_addr);

	myUrl.setHost("bob-xyz.com");
	test_verify(myUrl.encode() == "sip:bob-xyz.com");

	SipUrl myUrl2("", host_addr);
	myUrl2.setHost("bob-xyz.com");
	myUrl2.setUserValue("   ");
	test_verify(myUrl2.encode() == "sip:bob-xyz.com");
    }
    {
      Sptr<BaseUrl> xUrl = new SipUrl("sip:1000@192.168.5.111", host_addr);
      Sptr<BaseUrl> yUrl;

      yUrl = xUrl->duplicate();
      test_verify(yUrl->encode() == xUrl->encode());
    }

    {
        SipUrl url4(Data("sip:user:password@128.107.105.124:5060"), host_addr);
        test_verify(url4.getHost() == Data("128.107.105.124"));
        test_verify(url4.getUserValue() == Data("user:password"));
    }

    {
        SipUrl url4(Data("sip:user@128.107.105.124:5060"), host_addr);
        test_verify(url4.getHost() == Data("128.107.105.124"));
        test_verify(url4.getUserValue() == Data("user"));
    }

    {
        SipUrl url4(Data("sip:user@128.107.105.124"), host_addr);
        test_verify(url4.getHost() == Data("128.107.105.124"));
        test_verify(url4.getUserValue() == Data("user"));
    }

    {
      SipUrl url(Data("sip:1000@192.168.5.111"), host_addr);

      //      cout << url.getNameAddr() << endl;

      test_verify(url.getNameAddr() == "sip:1000@192.168.5.111");
    }
    {
      SipUrl url(Data("sip:[2002:ac17:fc4e::1]"), host_addr);
      test_verify(url.getNameAddr() == "sip:[2002:ac17:fc4e::1]");
    }
}


int main()
{
    test();
    return test_return_code(51);
}
