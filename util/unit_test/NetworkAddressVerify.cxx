
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
static const char* const NetworkAddressTest_cxx_Version =
    "$Id: NetworkAddressVerify.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";

#include "NetworkAddress.h"
#include "Verify.hxx"
#include "cpLog.h"

void test_00()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress item_1("192.168.5.100", 3700);

    test_verify(item_0 != item_1);
}

void test_01()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress item_1("192.168.5.100", 3400);

    test_verify(item_0 != item_1);
}


void test_02()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress item_1("192.168.5.254", 3400);

    test_verify (item_0 == item_1);
}

void test_03()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress* item_1 = new NetworkAddress("192.168.5.254", 3400);

    test_verify (*item_1 == item_0);
    delete item_1;
}

void test_04()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress* item_1 = new NetworkAddress("192.168.5.139", 3400);

    test_verify (!(*item_1 == item_0));
    delete item_1;
}

void test_05()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress* item_1 = new NetworkAddress("192.168.5.254", 3401);

    test_verify ( !(*item_1 == item_0));
}

void test_06()
{
    NetworkAddress na("vovida-www.cisco.com");
    NetworkAddress item_0(na.getIpName(), 5060);
    //    NetworkAddress::get
    test_verify(item_0.getHostName() == "vovida-www.cisco.com");

    test_verify(item_0.getIpName() == "128.107.250.40");

    NetworkAddress x(string("208.184.140.114"));
    test_verify(x.getHostName() == "208.184.140.114.available");
    cout << x.getHostName() << endl;    
}


int main()
{
//    cpLogSetPriority(LOG_DEBUG_STACK);
    test_00();
    test_01();
    test_02();
    test_03();
    test_04();
    test_05();
    test_06();

    return test_return_code(9);
}
