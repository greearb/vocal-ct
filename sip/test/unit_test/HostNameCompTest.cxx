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

static const char* const HostNameCompTest_cxx_Version =
    "$Id: HostNameCompTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <Data.hxx>
#include <cpLog.h>
#include <parse3tuple.h>
#include "InviteMsg.hxx"
#include "StatusMsg.hxx"
#include "AckMsg.hxx"
#include "ByeMsg.hxx"
#include "CancelMsg.hxx"
#include "RegisterMsg.hxx"
#include "SipTransceiver.hxx"
#include "SipContact.hxx"
#include "SipVia.hxx"
#include "Verify.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;


int main()
{
    //The url is :
    //goofy@vovida.com
    cpLogSetLabel ("Invitetest");
    cpLogSetPriority (LOG_DEBUG);

    Data url;
    //send invite to lab2
    url = "sip:skumar@192.168.5.12";
    SipUrl tourl(url, host_addr);

//    cout << "host is " << tourl.getHost().logData() << endl;
//    cout << "port is " << tourl.getPort().logData() << endl;


    Data url2;
    url2 = "sip:skumar@tesla.private.vovida.com";
    SipUrl toUrl2(url2, host_addr);

//    cout << "host is " << toUrl2.getHost().logData() << endl;
//    cout << "port is " << toUrl2.getPort().logData() << endl;



    cout << "Url comparison:  " << endl;
    test_verify (tourl != toUrl2);

    SipVia via1("", host_addr);
    via1.setHost("tesla.private.vovida.com");

    SipVia via2("", host_addr);
    via2.setHost("192.168.5.12");

    if (via1 == via2)
    {
        cout << "vias equal" << endl;
    }
    else
    {
        cout << "viasnot equal" << endl;
    }

    SipCallId callId1("1000@a.com", host_addr);
    callId1.setHost("tesla.private.vovida.com");

    SipCallId callId2 = callId1;
    callId2.setHost("192.168.5.12");


    if (callId1 == callId2)
    {
        cout << "callIds equal" << endl;
    }
    else
    {
        cout << "callIds not equal" << endl;
    }





    return test_return_code(1);
}
