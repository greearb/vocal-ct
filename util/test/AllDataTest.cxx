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
static const char* const AllDataTest_cxx_Version =
    "$Id: AllDataTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";

#include "Verify.hxx"
#include "CopyOnWriteData.hxx"

#include "StringData.hxx"
#include <iostream>



bool debug = true;

typedef char* (*func)();

bool test(func c_f, func s_f)
{
    char* c0 = c_f();
    char* s0 = s_f();

    bool ok =
        ( strcmp(c0, s0) == 0);

    if (debug)
    {
        cout << "c0: " << c0 << endl;
        cout << "s0: " << s0 << endl;
    }

    free(c0);
    free(s0);

    return ok;
}



template < class C >
char* test_00_helper()
{
    string a = "   asdf is the testing   thing  ";
    C test0 = a;

    test0.removeSpaces();

    return strdup(test0.getData());
}

bool test_00()
{
    return test(&test_00_helper < CopyOnWriteData > ,
                &test_00_helper < StringData > );
}


template < class C >
char* test_01_helper()
{
    C test0 = "Foo: a,b,c,d\r\n";

    test0.expand(C("Foo:"), C(","), C("\nFoo: "), C("\n"));
    return strdup(test0.getData());
}


bool test_01()
{
    return test(&test_01_helper < CopyOnWriteData > ,
                &test_01_helper < StringData > );
}


template < class C >
char* test_02_helper()
{
    C test0 = "random junk asdf this is a test";
    C test1;
    int retn = test0.match("asdf", &test1, false);

    assert(retn == FOUND);

    return strdup(test1.getData());
}

bool test_02()
{
    return test(&test_02_helper < CopyOnWriteData > ,
                &test_02_helper < StringData > );
}


template < class C >
char* test_03_helper()
{
    C test0 = "random junk asdf this is a test";
    C test1;
    int retn = test0.match("asdf", &test1, true);

    assert(retn == FOUND);

    return strdup(test0.getData());
}

bool test_03()
{
    return test(&test_03_helper < CopyOnWriteData > ,
                &test_03_helper < StringData > );
}


template < class C >
char* test_04_helper()
{
    C test0 = "random junk asdf this is a test";
    C test1;
    int retn = test0.match("asdf", &test1, true, "hello");

    assert(retn == FOUND);

    return strdup(test0.getData());
}

bool test_04()
{
    return test(&test_04_helper < CopyOnWriteData > ,
                &test_04_helper < StringData > );
}


template < class C >
char* test_05_helper()
{
    C test0 = "random junk asdf this is a test";
    C test1;
    int retn = test0.match("asdf", &test1, true, "hello");

    assert(retn == FOUND);

    return strdup(test1.getData());
}

bool test_05()
{
    return test(&test_05_helper < CopyOnWriteData > ,
                &test_05_helper < StringData > );
}




template < class C >
char* test_06_helper()
{
    string a = "asdf is the testing   thing";
    C test0 = a;

    test0.removeSpaces();

    return strdup(test0.getData());
}

bool test_06()
{
    return test(&test_06_helper < CopyOnWriteData > ,
                &test_06_helper < StringData > );
}



int main(int argc, char** argv)
{
    if (argc > 1)
    {
        debug = true;
    }

    test_verify(test_00());
    test_verify(test_01());
    test_verify( test_02() );
    test_verify( test_03() );
    test_verify( test_04() );
    test_verify( test_05() );
    test_verify( test_06() );

    return test_return_code(6);
}
