
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
static const char* const FifoTest_cxx_Version =
    "$Id: FifoTest03.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";
#include "Fifo.h"
#include "VThread.hxx"
#include "VTime.hxx"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

using namespace Vocal;

struct Foo
{
    bool isTimed;
    struct timeval start;
    Foo()
    {}


    Foo(const Foo& f) : isTimed(f.isTimed)
    {
        start.tv_sec = f.start.tv_sec;
        start.tv_usec = f.start.tv_usec;
    }
};

Fifo < Foo > shared;

int delay = 999;
int udelay = 10000;

void* run1(void*)
{
    while (1)
    {
        if (udelay)
            usleep(udelay);
        Foo x;
        x.isTimed = true;
        gettimeofday(&(x.start), 0);
        shared.addDelayMs(x, delay);
    }
    return 0;
}


void* run2(void*)
{
    int count = 0;
    int bad = 0;

    while (1)
    {
        Foo x = shared.getNext();
        if (x.isTimed)
        {
            // check to see if this is over 1 second
            struct timeval end;
            gettimeofday(&end, 0);

            long us =
                ( end.tv_sec - x.start.tv_sec) * 1000000 +
                ( end.tv_usec - x.start.tv_usec);

            if (us < (delay * 1000) )
            {
                bad++;
            }
        }
        count++;
        if ((count % 100) == 0)
        {
            printf("%d   %d\n", count, bad);
        }
    }
    return 0;
}



int main(int argc, char* argv[])
{

    if (argc > 1)
    {
        delay = atoi(argv[1]);
    }

    if (argc > 2)
    {
        udelay = atoi(argv[2]);
    }

    VThread thread1;
    VThread thread2;

    thread1.spawn(&run1, 0);
    thread2.spawn(&run2, 0);

    thread1.join();
    thread2.join();
    return 0;
}
