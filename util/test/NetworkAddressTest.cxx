
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
    "$Id: NetworkAddressTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";
#include "NetworkAddress.h"
#include "VTest.hxx"

bool test_00()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress item_1("192.168.5.100", 3700);

    return item_0 != item_1;
}

bool test_01()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress item_1("192.168.5.100", 3400);

    return !(item_0 == item_1);
}


bool test_02()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress item_1("192.168.5.254", 3400);

    return (item_0 == item_1);
}

bool test_03()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress* item_1 = new NetworkAddress("192.168.5.254", 3400);

    return (*item_1 == item_0);
}

bool test_04()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress* item_1 = new NetworkAddress("192.168.5.139", 3400);

    return !(*item_1 == item_0);
}

bool test_05()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress* item_1 = new NetworkAddress("192.168.5.254", 3401);

    return !(*item_1 == item_0);
}

bool test_06()
{
    NetworkAddress item_0("192.168.16.180", 5060);
    //    NetworkAddress::get
    return true;
}

bool test_07()
{
   NetworkAddress localhost;
   NetworkAddress otherhost("ottawa.gloo.net");
   return localhost.getIpName() != otherhost.getIpName();
}


bool test_08()
{
   try
   {
      NetworkAddress bogus("bogushost.foobar.yak.waka.com");
      bogus.getIpName();
   }
   catch (NetworkAddress::UnresolvedException& e)
   {
      return true;
   }
   return false;
}
bool test_09()
{
    return(NetworkAddress::is_valid_ip6_addr("ff14::1"));
}
bool test_10()
{
    return !(NetworkAddress::is_valid_ip6_addr("128.1.2.3:1"));
}
bool test_11()
{
    return !(NetworkAddress::is_valid_ip6_addr("128.1.2.3"));
}

int main()
{
    VTest myTest(__FILE__);

    myTest.test(0, test_00(), "NetworkAddress test 0");
    myTest.test(1, test_01(), "NetworkAddress test 1");
    myTest.test(2, test_02(), "NetworkAddress test 2");
    myTest.test(3, test_03(), "NetworkAddress test 3");
    myTest.test(4, test_04(), "NetworkAddress test 4");
    myTest.test(5, test_05(), "NetworkAddress test 5");
    myTest.test(6, test_06(), "NetworkAddress test 6");
    myTest.test(7, test_07(), "NetworkAddress test 7");
    myTest.test(8, test_08(), "NetworkAddress test 8");
    myTest.test(8, test_09(), "NetworkAddress test 9");
    myTest.test(8, test_10(), "NetworkAddress test 10");
    myTest.test(8, test_11(), "NetworkAddress test 11");

    NetworkAddress na;
    cerr << "IP name:" << na.getIpName().c_str() << endl;

    return myTest.exitCode();
}
