
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





static const char* const vsock_cxx_Version =
    "$Id: vsock.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "global.h"
#include "vsock.hxx"

#ifdef __vxworks
struct hostent* gethostbyname( const char* name )
{
    char buf[1024];
    return (hostent*)resolvGetHostByName( const_cast < char* > (name),
                                          buf, 1024 );
}

struct hostent* gethostbyaddr( const char* addr, int len, int type )
{
    char buf[1024];  // must be at least 512 bytes
    return (hostent*)resolvGetHostByAddr( addr, buf, 1024 );
}

unsigned int getpid( void )
{
    return (unsigned int)taskIdSelf();
}

#endif  // __vxworks


char* getUser( void )
{

#ifndef __vxworks
#ifdef WIN32
    static char user[64] = "unknown_user";
    unsigned long userSize = sizeof(user);
    GetUserName(user, &userSize);
    return (char*)user;

#else
    static char user[64] = "unknown_user";
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw != NULL)
    {
        strncpy(user, pw->pw_name, sizeof(user));
        user[63] = (char)0;
    }
    return (char*)user;
#endif
#else 
return (char*)sysBootParams.usr;
#endif  //  __vxworks
}


#ifdef __vxworks
int strcasecmp(const char* str1, const char* str2)
{
    unsigned char res;

    while (1)
    {
        if ( (res = (int)tolower(*str1) - (int)tolower(*str2++)) != 0 ||
                !*str1++ )
            break;
    }
    return res;
}


int strncasecmp(const char* str1, const char* str2, size_t len)
{
    unsigned char c1 = 0, c2 = 0;

    if ( len )
    {
        do
        {
            c1 = *str1++;
            c2 = *str2++;
            if (!c1) break;
            if (!c2) break;
            if ( c1 == c2 ) continue;
            c1 = tolower( c1 );
            c2 = tolower( c2 );
            if ( c1 != c2 ) break;
        }
        while (--len);
    }

    return (int)c1 - (int)c2;
}
#endif  //  __vxworks

