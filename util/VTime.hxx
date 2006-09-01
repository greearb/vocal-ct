#ifndef VTIME_H_
#define VTIME_H_

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





static const char* const VTimeHeaderVersion =
    "$Id: VTime.hxx,v 1.3 2006/09/01 19:51:02 greear Exp $";



#ifdef __vxworks

#include <time.h>
#include <sys/times.h>

/**
 ** Operates the same as linux gettimeofday() except the
 ** usec field of struct timeval is rounded to nearest msec
 ** (eg. usec = msec*1000)
 */
int
gettimeofday (struct timeval *tv, struct timezone *tz);

#else

#include "global.h"
#include "misc.hxx"
#include <sys/time.h>

#endif
// __vxworks

#include <sys/types.h>
#include "vtypes.h"
// stuff here
#include <string>


/** Class VTime
 ** Defines a time class which provides means of
 ** creating, adding, subtracting, and comparing times. 
 ** VTime objects contain a second component and a fraction of second
 ** component. Both may be specified and accessed by the user.
 ** Routines to add and subtract times in terms of milliseconds are
 ** also provided.
 **/
class VTime
{
    public:
        /// Create VTime object with specified seconds and fraction of a second.
        VTime( u_int32_t sec, u_int32_t frac ): seconds(sec), fractional(frac)
        {}

        /// Create a new VTime object with the current system time.
        VTime();

        /// Access functions
        u_int32_t getSeconds()
        {
            return seconds;
        }
        u_int32_t getFractional()
        {
            return fractional;
        }

        /// Display time stored in VTime in decimal notation
        void print();

        static string strftime(const string & format);


    private:
        // this is the numbers of seconds
        u_int32_t seconds;
        /** this is the fraction of the second.  1 second == 2^32 fractional .
         */
        u_int32_t fractional;

        /// add specified number of milliseconds to VTime
        friend VTime operator+( const VTime& lhs , const unsigned int msec );

        /// subtract specified number of milliseconds from VTime
        friend VTime operator-( const VTime& lhs , const unsigned int msec );

        /// returns difference in terms of milliseconds, or 0 if rhs > lhs
        friend int operator-( const VTime& lhs , const VTime& rhs );

        friend bool operator==( const VTime& lhs , const VTime& rhs );
        friend bool operator<( const VTime& lhs , const VTime& rhs );
        friend bool operator>( const VTime& lhs , const VTime& rhs );

};

/// returns a VTime object based on the current system time
VTime getVTime();


// V_TIME_H_
#endif
