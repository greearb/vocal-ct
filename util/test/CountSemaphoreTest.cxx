
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
static const char* const CountSemaphoreTestVersion =
    "$Id: CountSemaphoreTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";



#include "CountSemaphore.hxx"
#include <iostream>
#include "Verify.hxx"
#include "ThreadIf.hxx"

using namespace std;



void test()
{
    bool r;

    CountSemaphore s;
    assert( s.compare(0) );

    s.increment();
    assert( s.compare(1) );

    s.increment();
    assert( s.compare(2) );

    s.increment();
    assert( s.compare(3) );

    r = s.decrement();
    assert( s.compare(2) );
    assert( !r );

    r = s.decrement();
    assert( s.compare(1) );
    assert( !r );

    r = s.decrement();
    assert( s.compare(0) );
    assert( r );

    void* p1 = new char[50];
    void* p2 = new char[50];
    void* p3 = p1;
    void* p4 = p2;

    cout << p1 << endl;
    cout << p2 << endl;
    cout << p3 << endl;
    cout << p4 << endl;

    s.exchange(&p1, &p2);

    cout << p1 << endl;
    cout << p2 << endl;
    cout << p3 << endl;
    cout << p4 << endl;

    assert(p2 == p3);
    assert(p1 == p4);


}

CountSemaphore share;

void test2()
{
}

class T2 : public ThreadIf
{
    public:
        void thread()
        {
    for(int i = 0 ; i < 10000000; ++i)
    {
        share.increment();
        share.decrement();
    }
    }
};

int share2 = 0;

class T3 : public ThreadIf
{
    public:
        void thread()
        {
    for(int i = 0 ; i < 10000000; ++i)
    {
        share2++;
        share2--;
    }
    }
};


int main()
{
    test();
    assert(share.compare(0) == 1);
    cout << "pass" << endl;
    {
	T2 t1;
	T2 t2;
	T2 t3;
	T2 t4;
	t1.run();    
	t2.run();
	t3.run();
	t4.run();
	t1.join();    
	t2.join();    
	t3.join();    
	t4.join();    
    }

    assert(share2 == 0);
    {
	T3 t1;
	T3 t2;
	T3 t3;
	T3 t4;
	t1.run();    
	t2.run();
	t3.run();
	t4.run();
	t1.join();    
	t2.join();    
	t3.join();    
	t4.join();    
    }
    if(share2 != 0)
    {
	cout << "unprotected int corrupted: " << share2 << endl;
    }
    assert(share.compare(0) == 1);
    return 0;
}
