
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

static const char* const SipWwwAuthenticate_cxx_Version =
    "$Id: SipWwwAuthenticate.cxx,v 1.5 2006/03/12 07:41:28 greear Exp $";

#ifndef __MINGW32__
#pragma warning (disable: 4786)
#endif

#include <iostream>

#include "global.h"
#include "symbols.hxx"
#include "cpLog.h"
#include "SipWwwAuthenticate.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipWwwAuthenticateParserException::getName( void ) const
{
    return "SipWwwAuthenticateParserException";
}


SipWwwAuthenticate::SipWwwAuthenticate( const SipWwwAuthenticate& src )
    : SipHeader(src),
      authScheme(src.authScheme),
      myParamList(src.myParamList)
{
}


SipWwwAuthenticate::SipWwwAuthenticate( const Data& srcData, const string& local_ip )
    : SipHeader(srcData, local_ip),
      authScheme(),
      myParamList(',')
{

    if (srcData == "") {
        return;
    }

    Data fdata = srcData;
    decode(fdata);
}


SipWwwAuthenticate&
SipWwwAuthenticate::operator = (const SipWwwAuthenticate& src)
{
    if (&src != this) {
        authScheme = src.authScheme;
        myParamList= src.myParamList;
    }
    return (*this);
}


bool
SipWwwAuthenticate::operator ==(const SipWwwAuthenticate& src) const
{
    return ( ( isEqualNoCase(authScheme,src.authScheme) ) &&
             ( myParamList == src.myParamList) );
}


void
SipWwwAuthenticate::decode(const Data& data)
{
    Data nData = data;

    if (scanSipWwwauthorization(nData) < 0) {
       cpLog(LOG_ERR, "ERROR:  Failed to decode SipWwwAuthenticate, data -:%s:-\n",
             data.c_str());
    }
}


int SipWwwAuthenticate::scanSipWwwauthorization(const Data& tmpdata) {
   Data authdetails = tmpdata ;
   Data authType;
   int ret = authdetails.match(" ", &authType, true);
   if (ret == FIRST) {
      if (SipParserMode::sipParserMode()) {
         cpLog(LOG_ERR, "ERROR:  Failed to decode ProxyAuthenticate, tmpdata -:%s:-\n",
               tmpdata.c_str());
         return -1;
      }
   }
   else if (ret == FOUND) {
      setAuthScheme(authType);
      cpLog(LOG_DEBUG_STACK,"Found auth sceme [%s]", authType.logData());
      
      if ( ( isEqualNoCase(authType,AUTH_BASIC)) ||
           ( isEqualNoCase(authType,AUTH_DIGEST)) ||
           ( isEqualNoCase(authType,AUTH_PGP ) )
         ) {

         if (myParamList.decode(authdetails, ',') < 0) {
            if (SipParserMode::sipParserMode()) {
               cpLog(LOG_ERR, "ERROR:  Failed parsing auth tokens, tmpdata -:%s:-\n",
                     tmpdata.c_str());
               return -1;
            }
         }
      }
   }
   else if (ret == NOT_FOUND) {
      cpLog(LOG_ERR, "ERROR:  No Auth Scheme, tmpdata -:%s:-\n",
            tmpdata.c_str());
      return -1;
   }
   return 0;
}



/*** return the encoded string version of this. This call should only be
     used inside the stack and is not part of the API */
Data
SipWwwAuthenticate::encode() const
{
    Data data;

    if ( authScheme.length() )
    {
        data += SIP_WWWAUTHENTICATE;
        data += SP;
        data += authScheme;
        data += SP;

        data += myParamList.encode();

        data += CRLF;
    }

    return data;
}


///
void
SipWwwAuthenticate::setAuthTokenDetails(const Data& token,
        const Data& tokenValue)
{
    myParamList.setValue(token, tokenValue);
}


///
void
SipWwwAuthenticate::setRealmValue(const Data& data)
{
    myParamList.setValue(REALM, data);
}


///
Data
SipWwwAuthenticate::getTokenValue(const Data& token) const {
   Data ret;
   //Strip off any "" from the value
   string tokenstr = myParamList.getValue(token).convertString();
   int pos;
   pos = tokenstr.find("\"");
   
   if (pos != static_cast<int>(string::npos)) {
      tokenstr = tokenstr.substr(pos + 1, tokenstr.length() - 2);
   }
   
   ret = tokenstr;
   
   return ret;
}


///
Data
SipWwwAuthenticate::getRealmValue() const {
   return getTokenValue(REALM);
}



Sptr<SipHeader>
SipWwwAuthenticate::duplicate() const {
   return new SipWwwAuthenticate(*this);
}


bool SipWwwAuthenticate::compareSipHeader(SipHeader* msg) const {
   SipWwwAuthenticate* otherMsg = dynamic_cast<SipWwwAuthenticate*>(msg);
   if(otherMsg != 0) {
      return (*this == *otherMsg);
   }
   else {
      return false;
   }
}
