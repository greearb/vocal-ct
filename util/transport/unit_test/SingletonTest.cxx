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


static const char* const SingletonTest_cxx_Version = 
    "$Id: SingletonTest.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "Application.hxx"
#include "Singleton.hxx"
#include "Verify.hxx"


using namespace Vocal;


class SingletonTest : public Application
{
    public:
    
        SingletonTest() {}
                     
        virtual ~SingletonTest() {}

        ReturnCode  run();        
};


class SingletonTestCreator : public ApplicationCreator
{
    Application * create() { return ( new SingletonTest ); }
};


int main(int argc, char ** argv, char ** arge)
{
    SingletonTestCreator    creator;
    Application::creator(&creator);
    Application::main(argc, argv, arge);
    
    return ( test_return_code(4) );
}


class Foo : public Singleton<Foo>
{
    public:

        Foo() { test_verify(true); }

        virtual ~Foo() { test_verify(true); }
};


class Bar : public Singleton<Bar>
{
    public:

        Bar() { test_verify(true); }

        virtual ~Bar() { test_verify(true); }
};


class Baz : public Singleton<Baz>
{
    public:

        Baz() { test_verify(true); }

        virtual ~Baz() { test_verify(true); }
};


class Beh : public Singleton<Beh>
{
    public:

        Beh() { test_verify(true); }

        virtual ~Beh() { test_verify(true); }
};



ReturnCode  
SingletonTest::run()
{
    Foo & foo = Foo::instance();
    Bar & bar = Bar::instance();
    Baz & baz = Baz::instance();
    Beh & beh = Beh::instance();
    
    (void)foo; 
    (void)bar;
    (void)baz;
    (void)beh;

    try
    {
        Foo foo;
    }
    catch ( char * s )
    {
        cout << s << endl;
    }
    
    return ( SUCCESS );
}
