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


static const char* const TimeVal_cxx_Version =
    "$Id: TimeVal.cxx,v 1.2 2006/03/12 07:41:28 greear Exp $";


#include "global.h"
#include "TimeVal.hxx"
#include "VTime.hxx"
#include <cassert>


using Vocal::TimeAndDate::TimeVal;
using Vocal::TimeAndDate::milliseconds_t;


TimeVal::TimeVal()
{
    clear();
}


TimeVal::TimeVal(milliseconds_t src)
{
    clear();
    operator+=(src);
}

TimeVal::TimeVal(long sec, long usec)
{
    clear();
    tv_sec = sec;
    tv_usec = usec;
    normalize();
}
        


TimeVal::TimeVal(const timeval & src)
{
    copy(src);
    normalize();
}


TimeVal::~TimeVal()
{
}


TimeVal &
TimeVal::operator=(const timeval & src)
{
    if ( this != & src )
    {
        copy(src);
        normalize();
    }
    return ( *this );
}


const TimeVal &
TimeVal::now()
{
    int rc = vgettimeofday(this, 0);
    assert( rc == 0 );

    return ( *this );
}


milliseconds_t
TimeVal::milliseconds() const
{
    return ( (tv_sec * 1000) + (tv_usec / 1000) );
}


TimeVal
TimeVal::operator+(const timeval & right) const
{
    TimeVal left(*this);
    return ( left += right );
}


TimeVal
TimeVal::operator+(milliseconds_t right) const
{
    TimeVal left(*this);
    return ( left += right );
}


TimeVal &
TimeVal::operator+=(const timeval & src)
{
    tv_sec += src.tv_sec;
    tv_usec += src.tv_usec;

    normalize();

    return ( *this );
}


TimeVal &
TimeVal::operator+=(milliseconds_t src)
{
    tv_sec += src / 1000;
    tv_usec += (src % 1000) // Between [0..1000) milliseconds.
               * 1000;      // Convert to microseconds.

    normalize();

    return ( *this );
}


TimeVal
TimeVal::operator-(const timeval & right) const
{
    TimeVal left(*this);
    return ( left -= right );
}


TimeVal
TimeVal::operator-(milliseconds_t right) const
{
    TimeVal left(*this);
    return ( left -= right );
}


TimeVal &
TimeVal::operator-=(const timeval & src)
{
    tv_sec -= src.tv_sec;
    tv_usec -= src.tv_usec;

    normalize();

    return ( *this );
}


TimeVal &
TimeVal::operator-=(milliseconds_t src)
{
    tv_sec -= src / 1000;
    tv_usec += (src % 1000) // Between [0..1000) milliseconds.
               * 1000;      // Convert to microseconds.

    normalize();

    return ( *this );
}


bool
TimeVal::operator==(const timeval & src) const
{
    // Create a normalize value for the timeval to simplify comparison.
    //
    TimeVal right(src);

    return ( tv_sec == right.tv_sec
             && tv_usec == right.tv_usec
           );
}


bool
TimeVal::operator<(const timeval & src) const
{
    // Create a normalize value for the timeval to simplify comparison.
    //
    TimeVal right(src);

    if ( tv_sec < right.tv_sec )
    {
        return ( true );
    }
    else if ( (tv_sec == right.tv_sec) && (tv_usec < right.tv_usec) )
    {
        return ( true );
    }
    else
    {
        return ( false );
    }
}

bool
TimeVal::operator<(const TimeVal & right) const
{
    if ( tv_sec < right.tv_sec )
    {
        return ( true );
    }
    else if ( (tv_sec == right.tv_sec) && (tv_usec < right.tv_usec) )
    {
        return ( true );
    }
    else
    {
        return ( false );
    }
}


void
TimeVal::clear()
{
    tv_sec = tv_usec = 0;
}


void
TimeVal::copy(const timeval & src)
{
    tv_sec = src.tv_sec;
    tv_usec = src.tv_usec;
}


void
TimeVal::normalize()
{
    // The purpose of normalize is to ensure that the tv_usec field
    // is in [0,100000).
    //

    if ( tv_usec < 0 )
    {
        long num_sec = ( -tv_usec / 1000000) + 1;

        assert(num_sec > 0);

        tv_sec -= num_sec;
        tv_usec += num_sec * 1000000;

        // The result of this operation may have tv_usec == 1000000.
        //
    }


    if ( tv_usec >= 1000000 )
    {
        tv_sec += tv_usec / 1000000;
        tv_usec %= 1000000;
    }
}


ostream &
TimeVal::writeTo(ostream & out) const
{
    return ( out << "{ " << tv_sec << ", " << tv_usec << " }" );
}

