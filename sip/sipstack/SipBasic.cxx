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

static const char* const SipBasic_cxx_Version =
    "$Id: SipBasic.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipBasic.hxx"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "cpLog.h"
#include "support.hxx"
#include "Base64.hxx"

using namespace Vocal;

SipBasic::SipBasic()
{
}


SipBasic::~SipBasic()
{
}


Data SipBasic::formSIPBasic( const Data& user, const Data& pwd ) const
{
    //user:pwd- base64 encoded---RFC 2617


    unsigned char userbuf[1024];
    int userlen;


    unsigned char pwdbuf[1024];
    int pwdlen;

    unsigned char colonbuf[1024];
    int colonlen;


    //convert char* to unsigned char*.

    pwdlen = convertToUnsigned(pwd, pwdbuf);

    colonlen = convertToUnsigned(":", colonbuf);

    userlen = convertToUnsigned(user, userbuf);


    unsigned int concatValuelen;

    //malloc concatValue here.

    unsigned char* concatValue 
	= (unsigned char*)(malloc(pwdlen + colonlen + userlen + 1));

    Data basicCookie;

    if (concatValue)
    {
        memcpy(concatValue, userbuf, userlen);
        concatValuelen = userlen;

        memcpy(concatValue + concatValuelen, colonbuf, colonlen);
        concatValuelen += colonlen;

        memcpy(concatValue + concatValuelen, pwdbuf, pwdlen);
        concatValuelen += pwdlen;

        concatValue[concatValuelen] = '\0';

        //convert digesconcatenated value to base 64

        //allocate 2*concatValuelen.
        unsigned char* encodedBuf = (unsigned char*)(malloc(concatValuelen * 2));

        unsigned int encodedBuflen;


        if (encodedBuf)
        {
            int ret 
		= Vocal::Base64::encode((unsigned char*)encodedBuf, 
					&encodedBuflen, 
					(unsigned char*)concatValue, 
					concatValuelen);

            if (ret > 0)
            {
                basicCookie = (char*)(encodedBuf);
            }

            free(encodedBuf);
        }
        free(concatValue);
    }

    return basicCookie;
} 

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
