
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


static const char* const Timer_cxx_Version =
    "$Id: Timer.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "global.h"
#include "Timer.h"

/// compares two timevals
bool operator < ( const timeval& lhs, const timeval& rhs)
{
    bool ret;

    if (lhs.tv_sec == rhs.tv_sec)
    {
        ret = (lhs.tv_usec < rhs.tv_usec);
    }
    else
    {
        ret = (lhs.tv_sec < rhs.tv_sec);
    }
    return ret;
}

void addMs ( timeval* lhs, int timeMs)
{
    long timeUsec = timeMs * 1000;
    lhs->tv_usec += timeUsec;

    while (lhs->tv_usec > 1000000)
    {
        lhs->tv_sec += 1;
        lhs->tv_usec -= 1000000;
    }
}

void subMs ( timeval* lhs, int timeMs)
{
    long timeUsec = timeMs * 1000;
    long usecs = lhs->tv_usec - timeUsec;

    while (usecs < 0)
    {
        lhs->tv_sec -= 1;
        usecs += 1000000;
    }
    lhs->tv_usec = usecs;
}

long tv_diff ( const timeval& lhs, const timeval& rhs)
{
    return (lhs.tv_sec - rhs.tv_sec) * 1000000
           + ( lhs.tv_usec - rhs.tv_usec);
}
