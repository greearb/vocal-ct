
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
static const char* const ProfileTest_cxx_Version =
    "$Id: ProfileTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";
#include <assert.h>
#include "VThread.hxx"
#include <unistd.h>
#include <stdio.h>

void profile0()
{
    for (int i = 0; i < 50; i++);
}

void profile1()
{
    for (int i = 0; i < 100; i++);
}

#define THREAD_PROFILE 1

void* runner(void* proportion)
{
    /*
    int props = *((int *) proportion) | 0x7f;
    int runme = *((int *) proportion) & 0x80;
*/
    int props = 2;
    int runme = 2;

#if THREAD_PROFILE
    u_short buf[1000000];
    size_t addr = ((size_t) & profile0) - 100000;
    if (runme)
    {
        memset(buf, 0, 1000000);

        profil(buf, 1000000, addr, 65536);
    }
#endif

    int count0 = 0;
    int count1 = 0;

    for (int i = 0; i < 1000000; i++)
    {
        if (i % props)
        {
            profile0();
            count0++;
        }
        else
        {
            profile1();
            count1++;
        }
    }

#if THREAD_PROFILE
    if (runme)
    {
        printf ("%d\n", (int)addr);
        profil(0, 1000000, addr, 65536);
        for (int j = 0; j < 1000000; j++)
        {
            if (buf[j] > 0)
            {
                printf ("%d: %d: %d\n",
                        (int)VThread::selfId(),
                       (int)( addr + j), buf[j]);
            }
        }
    }
#endif
    return 0;
}

int main()
{
#if 1
#if !THREAD_PROFILE
    size_t addr = ((size_t) & profile0) - 100000;
    u_short buf[1000000];

    memset(buf, 0, 1000000);

    profil(buf, 1000000, addr, 65536);
#endif

    VThread thread1;
    VThread thread2;
    VThread thread3;
    VThread thread4;
    VThread thread5;
    static int	    vProportion1=2 | 0x80;
    static int	    vProportion2=2;
    
    thread1.spawn(&runner, (void*) &vProportion1);
    /*
    thread2.spawn(&runner, (void*) &vProportion2);
    thread3.spawn(&runner, (void*) &vProportion2);
    thread4.spawn(&runner, (void*) &vProportion2);
    thread5.spawn(&runner, (void*) &vProportion2);
*/
    thread1.join();
    /*
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
*/
#if !THREAD_PROFILE
    profil(0, 1000000, addr, 65536);
    for (int j = 0; j < 1000000; j++)
    {
        if (buf[j] > 0)
        {
            printf ("%d: %d: %d\n",
                    VThread::selfId(),
                    addr + j, buf[j]);
        }
    }
#endif

#else
runner((void*) 2);
#endif

    return 0;
}
