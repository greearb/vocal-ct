#ifndef FASTMEMCPY_HXX_
#define FASTMEMCPY_HXX_

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





static const char* const fastmemcpy_hxx_Version =
    "$Id: fastmemcpy.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#ifndef WIN32
#define USE_DUFFS_DEVICE 1
#define USE_MEMCPY 0
#define USE_FASTMEMCPY 0
#else
#define USE_DUFFS_DEVICE 0
#define USE_MEMCPY 1
#define USE_FASTMEMCPY 0
#endif

#ifndef INLINE_
#define INLINE_ inline
#endif

#if USE_FASTMEMCPY
INLINE_ void fastmemcopy(char* dest, char* src, int count)
{
    long long* s = reinterpret_cast < long long* > (src);
    long long* d = reinterpret_cast < long long* > (dest);
    int numberLongWords = (count + 7) / 8;

    while (numberLongWords--)
    {
        *(d++) = *(s++);
    }
}
#endif

#if USE_DUFFS_DEVICE
INLINE_ void* fastmemcopy(char* dest, char* src, int len)
{
    const long long* s = reinterpret_cast < const long long* > (src);
    long long* d = reinterpret_cast < long long* > (dest);

    int numberLongWords = (len + 7) / 8;

    int count = (numberLongWords + 7) / 8;

    switch (numberLongWords % 8)
    {
        case 0:
        do
        {
            *(d++) = *(s++);
            case 7:
            *(d++) = *(s++);
            case 6:
            *(d++) = *(s++);
            case 5:
            *(d++) = *(s++);
            case 4:
            *(d++) = *(s++);
            case 3:
            *(d++) = *(s++);
            case 2:
            *(d++) = *(s++);
            case 1:
            *(d++) = *(s++);
        }
        while (--count > 0);
    }
    return dest;
}
#endif

#if USE_MEMCPY
#define fastmemcopy(a,b,c) memcpy(a,b,c)
#endif


INLINE_ void mymemcopy(char* dest, const char* src, int count)
{
    memcpy(dest, src, count);
}

#endif
