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

static const char* const UrlParseTest_cxx_Version =
    "$Id: UrlParseTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "cpLog.h"
#include "symbols.hxx"
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <iostream.h>
#include "SipUrl.hxx"
#include "SipTo.hxx"
#include "SipFrom.hxx"
#include "SipCallId.hxx"
#include "SipCSeq.hxx"
#include "SipContentLength.hxx"
#include "SipContentType.hxx"
#include "SipContentEncoding.hxx"
#include "SipRoute.hxx"
#include "SipDate.hxx"
#include "SipContact.hxx"
#include "SipExpires.hxx"
#include "SipProxyAuthorization.hxx"
#include "SipMaxForwards.hxx"
#include "SipRequire.hxx"
#include "SipOrganization.hxx"
#include "SipTimestamp.hxx"
#include "SipAllow.hxx"
#include "SipSubject.hxx"
#include "SipAccept.hxx"
#include "SipRequestLine.hxx"
#include "SipStatusLine.hxx"
#include "InviteMsg.hxx"
#include "Verify.hxx"
#include "SipRawHeaderContainer.hxx"
#include "Sptr.hxx"
#include <vector>
using namespace Vocal;

int main(int argc, char *argv[])
{
    // UrlParseTest parseTest;
     Data url1("sip:a.g.bell@bell-telephone.com:5060");
    
    SipUrl sipUrl(url1);

    test_verify(sipUrl.encode() == url1);
    test_verify(sipUrl.getHost() == "bell-telephone.com");
    test_verify(sipUrl.getPort() == "5060");
    
    Data url2("sip:veer@vovida.com:5560;transport=udp?veeru=23");
    SipUrl sipUrl2(url2);

    test_verify(url2 == sipUrl2.encode());
    test_verify(sipUrl2.getHost() == "vovida.com");
    test_verify(sipUrl2.getPort() == "5560");
    test_verify(sipUrl2.getTransportParam() == "udp");
    cout << "transport param is:" << sipUrl2.getTransportParam().logData() << endl;

    cout << "other name is:" << sipUrl2.getOtherName().logData() << endl;
    cout << "other param is:" << sipUrl2.getOtherParam().logData() << endl;



    Data url3( "sip:12321321;isub=2142114;postd=2134*#-.2ppwww364@rock.com:5060;user=phone?header=we");

    SipUrl sipUrl3(url3);
    test_verify(sipUrl3.encode() == url3);
    cout << sipUrl3.encode() << endl;

    //test_verify

    Data url4("sip:+23229292@vovida.com;tag=2344");
    SipUrl sipUrl4(url4);
    test_verify(sipUrl4.encode() == url4);
    cout << sipUrl4.encode() << endl;
    
#if 0
    char* msg_invite = "INVITE sip:watson@boston.bell-telephone.com\r\nSIP/2.0\r\nVia: SIP/2.0/UDP 169.130.12.5\r\nFrom: <sip:a.g.bell@bell-telephone.com>\r\nTo: T. A. Watson <sip:watson@bell-telephone.com>\r\nCall-ID: 187602141351@worcester.bell-telephone.com\r\n";


    char* msg_info = "INFO sip:watson@boston.bell-telephone.com SIP/2.0\nVia: SIP/2.0/UDP 169.130.12.5\nFrom: <sip:a.g.bell@bell-telephone.com>\nTo: T. A. Watson <sip:watson@bell-telephone.com>\nCall-ID: 187602141351@worcester.bell-telephone.com\n";
#endif

#if 0
    Data fg = "sip:12321321;isub=2142114;postd=2134*#-.2ppwww364@rock.com:5060;user=phone?header=we";
    //Data fg = "sip:stones@vovida.com;ttl=25?subject=project%20x&prirty=urgent";
    //Data fg= "sip:veer@vovida.com:5060;transport=udp;user=ip;method=top?dg=flash";//;god";
    //	Data jk="sip:veer@vovida.com:5560;transport=udp?veeru=23";
    //Data hj = "Krishan Veer<sip:veer@vovida.com>;tag=111111";
    Data hj = " KRISHAN VEER<sip:veer@vovida.com>;tag=111111;mthod=777";
    //  Data hj = "Krishan Veer<sip:veer@vovida.com ";
    //    Data hj = "sip:+23229292@vovida.com;tag=23444";
    //Data hj = "Krishan Veer sip:veer@vovida.com>";
    SipUrl url1(fg);
    Data ifa = url1.encode();
    string sd = ifa;
    string fk = url1.getUser();
    cout << fk << endl;
    SipFrom url2(hj);
    Data tyu = url2.encode();
    string dfg = tyu;
    cout << "From after Decoding = " << dfg << endl;
    Data hj1 = "dsfagah-afdsggh-fadsgh@vovida.com";
    SipCallId url12(hj1);
    Data tyu2 = url12.encode();
    string dfg2 = tyu2;
    cout << "CallId after Decoding = " << dfg2 << endl;

    Data hk1 = "12323 CANCEL";
    SipCSeq ur(hk1);
    Data t3 = ur.encode();
    string d2 = t3;
    cout << "CallId after Decoding = " << d2 << endl;
    Data hk12 = "12323987654321";
    SipContentLength ur1(hk12);
    Data t31 = ur1.encode();
    string d21 = t31;
    cout << "CallId after Decoding = " << d21 << endl;
    Data hk123 = "text/html;charset=ISO-sjddds;crset=1232-3987654321";
    SipContentType ur13(hk123);
    Data t313 = ur13.encode();
    string d213 = t313;
    cout << "CallId after Decoding = " << d213 << endl;
#endif

#if 0
    //Data fg = "sip:veer@geek.com:8909;test=veer;transport=tcp;maddr=2;ttl=34?header=gfgfgfgf";
    Data fg1 = "10 ACK";

    try
    {
        //InviteMsg fg;
        //SipProxyAuthorization fg12;
        //fg12.setAuthstring(fg1);
        //fg.setProxyAuthorization(fg12);
        SipCSeq fg(fg1);
        string ifa = fg.encode();
        //string sd = ifa;
        // Data gk1="Ravs";
        // Data gk2="yp.com";
        // url1.setUser(gk1);
        // url1.setHost(gk2);
        // string gk3= url1.encode();
        //string fk = url1.getUser();
        //string fk1 = url1.getHost();
        //Data dt= url1.getPort();
        //string fk2=dt;
        cout << "Decoded test " << ifa << endl;
        //cout <<"decoded test "<<fk<<endl;
        //cout <<"Decoded test "<<fk1<<endl;
        //cout <<"Decoded test "<<fk2<<endl;
        // cout <<"Decoded test "<<gk3<<endl;
    }

    catch (SipContactParserException e)
    {
        string d = "opps()";
        cout << d << endl;
    }
#endif

    return test_return_code(9);
}
