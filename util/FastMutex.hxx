#ifndef _FAST_MUTEX_HXX
#define _FAST_MUTEX_HXX

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

static const char* const FastMutex_hxx_Version =
    "$Id: FastMutex.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#if defined(__i386__) || defined (__ia64__)|| defined(__INTEL_COMPILER)
#define VOCAL_FASTMUTEX_AVAILABLE 1
#endif


#if defined(VOCAL_FASTMUTEX_AVAILABLE)

#if defined(__ia64__)

#include "vtypes.h"
#define aSwap(x,X) \
{ \
__asm__ __volatile__ ("xchg1 %0=%1,%0 " :"=r"(x),"=m"(X) : "r"(x), "m"(X)); \
}

#else

#define aSwap(x,X) \
{ \
__asm__ __volatile__ ("lock; xchgl %1, %0 " :"=a"(x) : "m"(X), "a"(x)); \
}

#endif

class FastMutex
{
public:
    FastMutex();
    ~FastMutex();
    inline void lock()
	{
#if defined(__ia64__)
            u_int8_t localLock = 1;
#else
            int localLock = 1;
#endif
	    do{
		aSwap(localLock,X);
	    } while(localLock);
	}

    inline void unlock()
	{
	    X = 0;
	}

protected:
#if defined(__ia64__)
    u_int8_t X;
#else
    int X;
#endif

private:
    FastMutex(const FastMutex&);
    FastMutex& operator=(const FastMutex&);
};


inline FastMutex::FastMutex()
    :
    X(0)
{}


inline FastMutex::~FastMutex()
{}


#endif

#endif
