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
static const char* const AllDataPerformanceTest_cxx_Version =
    "$Id: AllDataPerformanceTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";

#include "Verify.hxx"
#include "CopyOnWriteData.hxx"
//#include "NewData.hxx"
#include "StringData.hxx"
#include <iostream>
#include "VTime.hxx"


bool debug = false;

typedef int (*func)(int);

void test(int count, func c_f, func s_f)
{
    int c = 0;
    int s = 0;
    if (c_f)
    {
        c = c_f(count);
    }
    if (s_f)
    {
        s = s_f(count);
    }
    cout << "c: " << c << endl;
    cout << "s: " << s << endl;
}



template < class C >
int test_00_helper(int count)
{
    C test0 = "random junk asdf this is a test";
    C test1;
    C test2 = "asdf";

    VTime start;
    for (int i = 0; i < count; i++)
    {
        test0.match(test2.getData(), &test1, false);
    }
    VTime end;
    return end - start;
}


void test_00()
{
    test(100000,
         &test_00_helper < CopyOnWriteData > ,
         &test_00_helper < StringData > );
}


template < class C >
int test_01_helper(int count)
{
    C test0 = "random junk asdf this is a test";
    C test1;
    C test2 = "asdf";

    VTime start;
    for(int i = 0; i < count; i++)
    {
        test0.match("asdf", &test1, false);
    }
    VTime end;
    return end - start;
}


void test_01()
{
    test(100000,
         &test_01_helper < CopyOnWriteData > ,
         &test_01_helper < StringData > );
}


template < class C >
int test_02_helper(int count)
{
    C test0 = "random junk asdf this is a test";
    C test1;
    C test2 = "asdf";

    VTime start;
    for(int i = 0; i < count; i++)
    {
        test0.find(test2);
    }
    VTime end;
    return end - start;
}


void test_02()
{
    test(100000,
         &test_02_helper < CopyOnWriteData > ,
         &test_02_helper < StringData > );
}


int main(int argc, char** argv)
{
    test_00();
    test_01();

    test_02();

    return 0;
}
