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
static const char* const DataPerformanceTest_cxx_Version =
    "$Id: DataPerformanceTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";
#include <string>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include "Data2.hxx"

typedef Data TestData;

void data_performance(int buflen, int iterations, bool void_cow)
{
    long char_time, string_time, data_time;

    char* buf = new char[buflen + 1];

    int i;

    for (i = 0; i < buflen; i++)
    {
        buf[i] = random() % 26 + 'A';
    }
    buf[buflen] = '\0';

    // this is the char* time

    struct timeval start, end;

    char* dest;

    gettimeofday(&start, 0);
    for (i = 0; i < iterations; i++)
    {
        dest = new char[buflen + 1];
        memcpy(dest, buf, buflen + 1);
        if (void_cow)
        {
            dest[0] = 'z';
        }
        delete[] dest;
    }
    gettimeofday(&end, 0);

    char_time = (end.tv_sec - start.tv_sec) * 1000000 +
                (end.tv_usec - start.tv_usec);

    // this is the string time

    string strsource = buf;

    string* strdest;

    gettimeofday(&start, 0);
    for (i = 0; i < iterations; i++)
    {
        strdest = new string;
        *strdest = strsource;
        if (void_cow)
        {
            (*strdest)[0] = 'z';
        }
        delete strdest;
    }
    gettimeofday(&end, 0);

    string_time = (end.tv_sec - start.tv_sec) * 1000000 +
                  (end.tv_usec - start.tv_usec);


    // this is the string time

    TestData datasource = buf;

    TestData* datadest;

    gettimeofday(&start, 0);
    for (i = 0; i < iterations; i++)
    {
        datadest = new TestData;
        *datadest = datasource;
        if (void_cow)
        {
            datadest->setchar(0, 'z');
        }
        delete datadest;
    }
    gettimeofday(&end, 0);

    data_time = (end.tv_sec - start.tv_sec) * 1000000 +
                (end.tv_usec - start.tv_usec);

    ////////////////////////////////////////////////////////////
    // results

    printf(
        "Len: %7d  Iter: %7d  cow: %7s  char*: %8ld  string: %8ld  data: %8ld\n",
        buflen, iterations,
        (void_cow ? "invalid" : "ok"), char_time, string_time, data_time);

    delete[] buf;
    return ;
}


int main()
{
    data_performance(1, 1, false);

    data_performance(10, 10000, false);
    data_performance(100, 10000, false);
    data_performance(10000, 10000, false);
    data_performance(100000, 10000, false);


    data_performance(10000, 10, false);
    data_performance(10000, 100, false);
    data_performance(10000, 1000, false);
    data_performance(10000, 10000, false);
    data_performance(10000, 100000, false);

    data_performance(10, 10000, true);
    data_performance(100, 10000, true);
    data_performance(1000, 10000, true);
    data_performance(10000, 10000, true);

    return 0;
}
