
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



#include "global.h"
#include <cassert>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include "VTime.hxx"


/// Create a new VTime object with the current system time.
VTime::VTime() {
   *this = getVTime();
}


void VTime::print()
{
    fprintf( stderr, "%u.%010u", seconds, fractional );
}


VTime operator+( const VTime& lhs , const unsigned int msec )
{
    VTime result ( 0, 0 );

    u_int32_t delayFrac = ( msec % 1000 ) * 4294967;

    result.seconds = lhs.seconds + ( msec / 1000 );
    result.fractional = lhs.fractional + delayFrac;
    if ( ( lhs.fractional > result.fractional ) &&
            ( delayFrac > result.fractional ) )
    {
        result.seconds++;
    }
    return result;
}


VTime operator-( const VTime& lhs , const unsigned int msec )
{
    VTime result( 0, 0 );
    u_int32_t delayFrac = ( msec % 1000 ) * 4294967;

    if (lhs.seconds > (msec / 1000))
    {
        result.seconds = lhs.seconds - ( msec / 1000 );
        result.fractional = lhs.fractional - delayFrac;
        if ( delayFrac > lhs.fractional )
        {
            result.seconds--;
        }
    }
    else
    {
        result.seconds = 0;
        if ( delayFrac >= lhs.fractional )
        {
            result.fractional = 0;
        }
        else
        {
            result.fractional = lhs.fractional - delayFrac;
        }
    }
    return result;
}


int operator-( const VTime& lhs , const VTime& rhs )
{
    // partialresult is ms difference
    VTime result;
    int msResult;

    //assert (lhs.seconds >= rhs.seconds);
    if ( lhs == rhs ) return 0;
    //    if ( lhs < rhs ) return 0;

    if ( lhs > rhs )
    {
        result.seconds = lhs.seconds - rhs.seconds;

        if ( lhs.fractional >= rhs.fractional )
        {
            result.fractional = lhs.fractional - rhs.fractional;
        }
        else
        {
            result.seconds--;
            result.fractional = rhs.fractional - lhs.fractional;
        }
        msResult = ( ( result.getSeconds() * 1000 ) +
                     ( result.getFractional() / 4294967 ));
    }
    else
    {
        result.seconds = rhs.seconds - lhs.seconds;

        if ( rhs.fractional >= lhs.fractional )
        {
            result.fractional = rhs.fractional - lhs.fractional;
        }
        else
        {
            result.seconds--;
            result.fractional = lhs.fractional - rhs.fractional;
        }
        msResult = - ( ( ( result.getSeconds() * 1000 ) +
                         ( result.getFractional() / 4294967 ) ) );
    }

    return msResult;
}


bool operator==( const VTime& rhs , const VTime& lhs )
{
    return ( rhs.seconds == lhs.seconds ) ?
           ( rhs.fractional == lhs.fractional ) : ( rhs.seconds == lhs.seconds );
}

bool operator<( const VTime& rhs , const VTime& lhs )
{
    return ( rhs.seconds == lhs.seconds ) ?
           ( rhs.fractional < lhs.fractional ) : ( rhs.seconds < lhs.seconds );
}


bool operator>( const VTime& rhs , const VTime& lhs )
{
    return ( rhs.seconds == lhs.seconds ) ?
           (rhs.fractional > lhs.fractional ) : ( rhs.seconds > lhs.seconds );
}


VTime getVTime()
{
    struct timeval now;
    int err = vgettimeofday( &now, NULL );
    assert( !err );

    VTime result ( now.tv_sec, now.tv_usec * 4294 );

    return result;
}

string
VTime::strftime(const string& format)
{
    time_t now;

    (void)time(&now);

    char datebuf[256];

#if !defined(__APPLE__) && !defined(__MINGW32__)
    struct tm localt;
    ::strftime(datebuf, 256, format.c_str(), localtime_r(&now, &localt));
#else
    ::strftime(datebuf, 256, format.c_str(), localtime(&now));
#endif
    return (datebuf);
}


#ifdef __vxworks
/*********************************************************************
 
 subroutines for VxWorks
 
*********************************************************************/

#include <netinet/in.h>
#include <netinet/in_systm.h>

/*
 * this implementation of gettimeoftime() uses iptime()
 * which is accurate to the nearest millisecond; microsecond
 * accuracy is faked by multiplying the milliseconds by 1000
 */
int
gettimeofday (struct timeval *tv, struct timezone *tz)
{
    unsigned long int msec;  // msec since 00:00 GMT

    // iptime() returns msec since 00:00 GMT in reverse byte order
    // call ntohl to swap it
    msec = ntohl ( iptime() );
    tv->tv_sec = ( msec / 1000 );
    tv->tv_usec = ( msec % 1000 ) * 1000;
    return 0;
}

#endif // __vxworks
