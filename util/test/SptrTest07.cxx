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

static const char* const SptrTest07_cxx_Version =
    "$Id: SptrTest07.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";

#include "Sptr.hxx"
#include "Verify.hxx"
#include "ThreadIf.hxx"
#include <unistd.h>
#include <list>

class Test
{
    public:
	Test()
	{
            cout << "constructor" << endl;
	}

	~Test()
	{
            cout << "destructor" << endl;
	}
};

Sptr<Test> a;


class Start : public ThreadIf
{
    public:
        void thread()
        {
            while(!isShutdown())
            {
                {
                    list <Sptr <Test> > x;
                    for(int i = 0 ; i < 10000; i++)
                    {
                        x.push_back(a);
                    }
                    assert(!a.getCount()->compare(1));
                }
                if(a.getCount()->compare(1))
                {
                    cout << "ding!" << endl;
                }
            }
        }
    
};


// test 2 -- this is construction of dual "parallel" Sptrs that would happen
// using the constructor


int main()
{
    /// test items

    a = new Test;

    Start w;
    Start x;
    Start y;

    w.run();
    x.run();
    y.run();

    sleep(60);
    w.shutdown();
    x.shutdown();
    y.shutdown();

    w.join();
    x.join();
    y.join();

    test_verify(a.getCount()->compare(1));

    return test_return_code(1);
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
