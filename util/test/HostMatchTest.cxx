
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
static const char* const HostMatchTest_cxx_Version =
    "$Id: HostMatchTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";
// test VTest class

#include "VTest.hxx"
#include "HostMatch.hxx"


bool test_00()
{
    return hostsEqual("mail", "mail.utanet.at");
}

bool test_01()
{
    return !hostsEqual("www.yahoo.com", "www.utanet.at");
}


bool test_02()
{
    return !hostsEqual("www", "mail");
}

bool test_03()
{
    return hostsIntersect("www", "www.utanet.at");
}

bool test_04()
{
    return hostsIntersect("mail", "www");
}

bool test_05()
{
    return hostsIntersect("mail", "195.70.253.8");
}

bool test_06()
{
    return hostsIntersect("www", "213.90.34.33");
}

bool test_07()
{
    return hostsEqual("www.utanet.at", "213.90.34.33");
}

bool test_08()
{
    return !hostsIntersect("workstation", "millikan");
}

bool test_09()
{
    return !hostsIntersect("213.90.34.33", "195.70.253.8");
}

bool test_10()
{
    return !hostsEqual("213.90.34.33", "213.90.34.34");
}

bool test_11()
{
    return !hostsEqual("213.90.34.33", "213.90.34.");
}

bool test_12()
{
    return hostsEqual("213.90.34.33", "213.90.34.33");
}

bool test_13()
{
    return !hostsIntersect("213.90.34.33", "zxvohihewwthoihaoisdhiqotw.");
}

bool test_14()
{
    return !hostsEqual("adsihocxh 38h 3", "zxvohihewwthoihaoisdhiqotw.");
}

bool test_15()
{
    return hostsEqual("213.90.34.33", "213.90.34.33");
}

// test addresses only too

int main()
{
    VTest myTest(__FILE__);

    myTest.test(0, test_00(), "HostMatch test 0");
    myTest.test(1, test_01(), "HostMatch test 1");
    myTest.test(1, test_02(), "HostMatch test 2");
    myTest.test(1, test_03(), "HostMatch test 3");
    myTest.test(1, test_04(), "HostMatch test 4");
    myTest.test(1, test_05(), "HostMatch test 5");
    myTest.test(1, test_06(), "HostMatch test 6");
    myTest.test(1, test_07(), "HostMatch test 7");
    myTest.test(1, test_08(), "HostMatch test 8");
    myTest.test(1, test_09(), "HostMatch test 9");
    myTest.test(1, test_10(), "HostMatch test 10");
    myTest.test(1, test_11(), "HostMatch test 11");
    myTest.test(1, test_12(), "HostMatch test 12");
    myTest.test(1, test_13(), "HostMatch test 13");
    myTest.test(1, test_14(), "HostMatch test 14");
    myTest.test(1, test_15(), "HostMatch test 15");

    return myTest.exitCode();
}
