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

static const char* const DataMultiThread_cxx_Version =
    "$Id: DataMultiThread.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";

#include <unistd.h>
#include "Data.hxx"
#include "Verify.hxx"
#include "VThreadPool.hxx"

inline int randomInt(int max)
{
    return random() % max;
}

enum numberCases
{
    INITIALIZE_ALL,
    INITIALIZE_ONE,
    INITIALIZE_TWO,
    COPY_ONE_TWO,
    COPY_TWO_ONE,
    COMPARE_EQUAL,
    COMPARE_UNEQUAL,
    ASSIGN_C_STRING,
    APPEND_TWO,
    APPEND_C_STRING,
    NOACTION
};


inline char randomChar()
{
    return (32 + randomInt(126 - 32));
} 

inline bool operate(Data& data1, Data& data2)
{
    bool result = false;

    switch(randomInt(NOACTION))
    {
    case INITIALIZE_ALL:
	data1 = "";
	data2 = "";
	break;
    case INITIALIZE_ONE:
	data1 = "";
	break;
    case INITIALIZE_TWO:
	data2 = "";
	break;
    case COPY_ONE_TWO:
	data1 = data2;
	break;
    case COPY_TWO_ONE:
	data2 = data1;
	break;
    case COMPARE_EQUAL:
	result = data1 == data2;
	break;
    case COMPARE_UNEQUAL:
	result = data1 != data2;
	break;
    case ASSIGN_C_STRING:
    {
	int buflen = randomInt(10);
	char buf[buflen+1];
	for(int i = 0; i < buflen; i++)
	{
	    buf[i] = randomChar();
	}
	buf[buflen] = '\0';
	data1 = buf;
	break;
    }
    case APPEND_TWO:
    {
	data1 += data2;
	break;
    }
    case APPEND_C_STRING:
    {
	int buflen = randomInt(10);
	char buf[buflen+1];
	for(int i = 0; i < buflen; i++)
	{
	    buf[i] = randomChar();
	}
	buf[buflen] = '\0';
	data1 += buf;
	break;
    }
    case NOACTION:
    default:
	break;
    }
    return result;
}


Data one;
Data two;

void operateThread(void *)
{
    while (1)
    {
	operate(one, two);
	operate(two, one);
    }
}



int main()
{
    try
    {
        VThreadPool pool(5);


	VFUNC function = operateThread;

	
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

    return 0;
}
