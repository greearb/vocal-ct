
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
static const char* const DatathreadTest_cxx_Version =
    "$Id: DatathreadTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "cpLog.h"
//#include "PepAgent.hxx"
#include "VThreadPool.hxx"
//#include "CommandLine.hxx"
#include "Data.hxx"
Data testdata = "RED WINGS RULES!!!";

void loopingWriter(void*)
{
    while(1)
    {
	Data dupdata = testdata;
	LocalScopeAllocator lo;
        cout << "writer thread" << pthread_self() << dupdata.getData(lo) << endl;
        dupdata = "";
        dupdata += "RED WINGS RULES!!!";
        cout << "writer thread" << pthread_self() << dupdata.getData(lo) << endl;
    }
}


void loopingBadWriter(void *)
{
    while (1)
    {
        testdata = "";
        testdata += "RED WINGS RULES!!!";
	LocalScopeAllocator lo;
        cout << "writer thread " << pthread_self() << testdata.getData(lo) << endl;
    }
}


void readerThread(void*)
{
    //    cpLog(LOG_DEBUG, "Reader thread (%d)", pthread_self());
    int i = 5;
    while (i)
    {
	LocalScopeAllocator lo;
        cout << "reader thread" << pthread_self() << testdata.getData(lo) << endl;
    }
}


void writerThread(void *)
{
    int i = 5;
    while (i)
    {
        testdata = "";
        testdata += "RED WINGS RULES!!!";
	LocalScopeAllocator lo;
        cout << "writer thread " << pthread_self() << testdata.getData(lo) << endl;
    }
}

int
main(int argc, const char** argv)
{
    try
    {
        VThreadPool pool(5);


	VFUNC function = loopingWriter;

	
        VFunctor functor1(function, NULL);
        pool.addFunctor(functor1);

        VFunctor functor2(function, NULL);
        pool.addFunctor(functor2);

        VFunctor functor3(function, NULL);
        pool.addFunctor(functor3);

        VFunctor functor4(function, NULL);
        pool.addFunctor(functor4);

        VFunctor functor5(function, NULL);
        pool.addFunctor(functor5);


        while (true)
        {
            sleep(1);
        }
    }
    catch (VException& e)
    {
        cerr << "Caught exception:" << e.getDescription() << endl;
        exit( -1);
    }
}
