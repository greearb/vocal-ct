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

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <iomanip>

#include "TickCount.hxx"
#include "Duration.hxx"
#include "ScopedDuration.hxx"

using namespace std;
using Vocal::Statistics::TickCount;
using Vocal::Statistics::Duration;
using Vocal::Statistics::ScopedDuration;

inline void foo()
{
    int i; (void)i;
}


void bar()
{
    unsigned int i; (void)i;
}


void feh(int i)
{
    for (int n = 0; n < i; n++ )
    {
    	bar();
    }
}

int logging = 1;

#define COUT  if ( !logging ); else cout << setw(20)

int main(int argc, char** argv)
{
    if ( argc > 1 )
    {
    	logging = !logging;
    }

    Duration    main_d;
    int64_t	total = 0;

    {
    	ScopedDuration	sd(main_d);
	
	Duration d;
	TickCount start, end, tc;

	TickCount::calibrate();

	main_d.start();

	COUT << "Ticks per second: " << TickCount::ticksPerSec << endl;

	start.set();
	sleep(0);
	end.set();

	COUT << "sleep(0): " << end - start << endl;
	total += d.length();

	start.set();
	end.set();

	COUT << "Noop: " << end - start << endl;
	total += d.length();

	d.start();
	d.stop();

	COUT << "Noop: " << d.length() << endl;
	total += d.length();

	start.set();
	tc.set();
	end.set();

	COUT << "TickCounter::set(): " << end - start << endl;
	total += d.length();

	d.start();
	int i = 0;
	d.stop();

	COUT << "int construction: " << d.length() << endl;
	total += d.length();

	d.start();
	i++;
	d.stop();

	COUT << "int increment: " << d.length() << endl;
	total += d.length();

	d.start();
	times(0);
	d.stop();

	COUT << "times: " << d.length() << endl;
	total += d.length();

	timeval tv;

	d.start();
	gettimeofday(&tv, 0);
	d.stop();

	COUT << "gettimeofday: " << d.length() << endl;
	total += d.length();

	d.start();
	foo();
	d.stop();

	COUT << "inline foo: " << d.length() << endl;
	total += d.length();

	d.start();
	foo();
	d.stop();

	COUT << "inline foo: " << d.length() << endl;
	total += d.length();

	d.start();
	foo();
	d.stop();

	COUT << "inline foo: " << d.length() << endl;
	total += d.length();

	d.start();
	bar();
	d.stop();

	COUT << "bar: " << d.length() << endl;
	total += d.length();

	d.start();
	bar();
	d.stop();

	COUT << "bar: " << d.length() << endl;
	total += d.length();

	d.start();
	bar();
	d.stop();

	COUT << "bar: " << d.length() << endl;
	total += d.length();

	d.start();
	feh(10);
	d.stop();

	COUT << "10 func calls: " << d.length() << endl;
	total += d.length();

	d.start();
	feh(20);
	d.stop();

	COUT << "20 func calls: " << d.length() << endl;
	total += d.length();

	int k = 8;
	unsigned j;    

	d.start();
	j = static_cast<unsigned int>(k);
	d.stop();

	COUT << "static cast: " << d.length() << endl;
	total += d.length();

	d.start();
	printf("printf()\n");
	d.stop();

	COUT << ": " << d.length() << endl;
	total += d.length();

	d.start();
	cout << "cout ..." << endl;
	d.stop();

	COUT << ": " << d.length() << endl;
	total += d.length();

	d.start();
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	d.stop();

	COUT << "socket(): " << d.length() << endl;
	total += d.length();

	d.start();
	close(fd);
	d.stop();

	COUT << "close(socket): " << d.length() << endl;
	total += d.length();

	d.start();
	fd = open("foo.txt", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	d.stop();

	COUT << "open(): " << d.length() << endl;
	total += d.length();

	char  s[] = "Testing";

	d.start();
	write(fd, s, sizeof(s)-1);
	d.stop();

	COUT << "write(): " << d.length() << endl;
	total += d.length();

	d.start();
	close(fd);
	d.stop();

	COUT << "close(file): " << d.length() << endl;
	total += d.length();

	d.start();
	unlink("foo.txt");
	d.stop();

	COUT << "unlink(file): " << d.length() << endl;
	total += d.length();
    }

    cout << endl << setw(20) << "main: " << main_d.length() << endl;
    cout << setw(20) << "total: " << total << endl;        

    return ( 0 );
}
