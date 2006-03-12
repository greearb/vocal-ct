#ifndef GLOBAL_H_
#define GLOBAL_H_

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


/*
$Id: global.h,v 1.2 2006/03/12 07:41:28 greear Exp $
*/

// 25/11/03 fpi
// WorkAround Win32
#ifdef WIN32

#ifndef __MINGW32__
#pragma warning(disable : 4786)
#pragma warning (disable: 4290)
#pragma warning (disable: 4715)
#endif

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
namespace Vocal{};

#endif

#ifdef USE_DMALLOC
#include <dmalloc.h>
#endif

#ifdef __sgi
#include <pthread.h>
#include <string>
#endif

#ifdef __cplusplus
namespace std 
{
};

namespace __gnu_cxx
{
};

using namespace std;
using namespace __gnu_cxx;

#endif

/* this is used to turn of unused warnings in GCC */

#ifdef __GNUC__
#define UNUSED_VARIABLE __attribute__((__unused__))
#else
#define UNUSED_VARIABLE 
#endif

// Add: Mac OS X Support
#ifdef __APPLE__
#define in_addr_t u_int

#endif

#if defined(WIN32)

#if _MSC_VER > 1000
#pragma once
#endif /* _MSC_VER > 1000 */

#ifndef __MINGW32__
#pragma warning(disable : 4786)
#endif

#define WIN32_LEAN_AND_MEAN	

#include <winsock2.h>
#include <direct.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <process.h>

#define srandom         srand
#define random          rand

#define strcasecmp      stricmp
#define strncasecmp     strnicmp

#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 ENOMEM
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE

#if !defined(MAXHOSTNAMELEN)
#define MAXHOSTNAMELEN  256
#endif /* !defined(MAXHOSTNAMELEN) */

typedef int	            sigset_t;
typedef int                 siginfo_t;
typedef int                 pid_t;
typedef unsigned long int   in_addr_t;
typedef long                off_t;

struct pollfd
{
    int fd;
    short int events;
    short int revents;
};

#define POLLIN		0x001
#define POLLPRI		0x002
#define POLLOUT		0x004
#define POLLERR		0x008
#define POLLHUP		0x010
#define POLLNVAL	0x020

#define getcwd          _getcwd

#endif /* defined(WIN32) */

#endif /* GLOBAL_H_ */
