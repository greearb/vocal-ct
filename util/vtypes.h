#ifndef VTYPES_H_
#define VTYPES_H_

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

static const char* const vtypes_h_Version =
    "$Id: vtypes.h,v 1.1 2004/05/01 04:15:33 greear Exp $";

#if defined(__vxworks)

/* VxWorks types */
#include <time.h>
#include <sys/times.h>

#include "types/vxTypesOld.h"

#ifndef u_int32_t
typedef UINT32 u_int32_t;
#endif

#ifndef UWORD32
typedef UINT32 UWORD32;
#endif

#ifndef u_int16_t
typedef UINT16 u_int16_t;
#endif

#ifndef u_int8_t
typedef UINT8 u_int8_t;
#endif

#elif defined(__sparc)
#include <inttypes.h>

/* typedef unsigned char u_int8_t; */
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;

#elif defined(__svr4__)

#include <pthread.h>

#ifndef u_int32_t
typedef uint32_t u_int32_t;
#endif

#ifndef u_int8_t
typedef uint8_t u_int8_t;
#endif

#elif defined (WIN32)
#include "windows.h"

#ifndef u_int32_t
typedef UINT32  u_int32_t;
#endif

#ifndef u_int16_t
typedef USHORT  u_int16_t;
#endif

#ifndef u_int8_t
typedef UCHAR  u_int8_t;
#endif

#ifndef socklen_t
typedef INT32 socklen_t;
#endif

#ifndef int64_t
typedef INT64 int64_t;
#endif
#else
#include <sys/types.h>
#endif
/* endif machine type */


#endif
