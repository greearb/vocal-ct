#ifndef NTP_HXX
#define NTP_HXX

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



#include <sys/types.h>
#include "vtypes.h"
#include <misc.hxx>

class NtpTime;

NtpTime getNtpTime();

///
class NtpTime
{
    public:
        ///
        NtpTime( u_int32_t sec, u_int32_t frac ): seconds(sec), fractional(frac)
        {}
        ;
        ///
        NtpTime()
        {
            *this = getNtpTime();
        };
        ///
        u_int32_t getSeconds()
        {
            return seconds;
        };
        ///
        u_int32_t getFractional()
        {
            return fractional;
        };

        uint64 getUs() const {
           uint64 rv = ((uint64)(seconds)) * 1000000;
           rv += (fractional / 4294);
           return rv;
        }

        uint64 getMs() const {
           uint64 rv = ((uint64)(seconds)) * 1000;
           rv += (fractional / 4294967);
           return rv;
        }

        ///
        void print();
    private:
        u_int32_t seconds;
        u_int32_t fractional;

        friend NtpTime operator+( const NtpTime& rhs , const unsigned int delayMs );
        friend NtpTime operator-( const NtpTime& rhs , const unsigned int ms );
        friend int64 operator-( const NtpTime& rhs , const NtpTime& lhs );
        friend bool operator==( const NtpTime& rhs , const NtpTime& lhs );
        friend bool operator<( const NtpTime& rhs , const NtpTime& lhs );
        friend bool operator>( const NtpTime& rhs , const NtpTime& lhs );
};


#endif // NTP_HXX
