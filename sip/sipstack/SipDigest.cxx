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

static const char* const SipDigest_cxx_version =
    "$Id: SipDigest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipDigest.hxx"
#include "cpLog.h"
#include "support.hxx"

#define HASHLEN 16

using namespace Vocal;

SipDigest::SipDigest()
{
}


SipDigest::~SipDigest()
{
}


Data 
SipDigest::form_SIPdigest( const Data& nonce, 
                           const Data& user,
			   const Data& pwd, 
                           const Data& method,
			   const Data& requestURI,
			   const Data& realm,
			   const Data& qop,
			   const Data& cnonce,
			   const Data& alg,
			   const Data& noncecount)
{
    HASHHEX HA1;
    HASHHEX HA2 = "";
    HASHHEX response;

    char algstr[1024];
    char userstr[1024];
    char realmstr[1024];
    char pwdstr[1024];
    char noncestr[1024];
    char cnoncestr[1024];
    char noncecountstr[1024];
    char qopstr[1024];
    char methodstr[1024];
    char reqURIstr[1024];

    alg.getData(algstr,1024);
    user.getData(userstr,1024);
    realm.getData(realmstr,1024);
    pwd.getData(pwdstr,1024);
    nonce.getData(noncestr,1024);
    cnonce.getData(cnoncestr,1024);
    noncecount.getData(noncecountstr,1024);
    qop.getData(qopstr,1024);
    method.getData(methodstr,1024);
    requestURI.getData(reqURIstr,1024);

    DigestCalcHA1(algstr, userstr, realmstr, pwdstr, noncestr, cnoncestr, HA1);
    DigestCalcResponse(HA1, 
                       noncestr, 
                       noncecountstr, 
                       cnoncestr, 
                       qopstr, 
                       methodstr, 
                       reqURIstr, 
                       HA2, 
                       response);

    cpLog(LOG_DEBUG_STACK, "Response = %s\n", response);

    return response;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
