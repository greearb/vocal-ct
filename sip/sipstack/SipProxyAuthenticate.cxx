
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

static const char* const SipProxyAuthenticate_cxx_Version =
    "$Id: SipProxyAuthenticate.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "cpLog.h"
#include "SipProxyAuthenticate.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipProxyAuthenticateParserException::getName( void ) const
{
    return "SipProxyAuthenticateParserException";
}


SipProxyAuthenticate::SipProxyAuthenticate( const SipProxyAuthenticate& src )
    : SipHeader(src),
      authScheme(src.authScheme),
      myParamList(src.myParamList)
{
}


SipProxyAuthenticate::SipProxyAuthenticate( const Data& srcData, const string& local_ip )
    : SipHeader(srcData, local_ip), 
      authScheme(),
      myParamList(',')
{
    if (srcData == "") {
        return;
    }
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipProxyAuthenticateParserException&)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipProxyAuthenticateParserException(
                "failed to decode the ProxyAuthenticate string",
                __FILE__,
                __LINE__,
                DECODE_FAILED_PROXYAUTHENTICATE);
        }
    }
}


SipProxyAuthenticate&
SipProxyAuthenticate::operator = (const SipProxyAuthenticate& src)
{
    if (&src != this)
    {
        authScheme = src.authScheme;
        myParamList= src.myParamList;
    }
    return (*this);
}


bool
SipProxyAuthenticate::operator ==(const SipProxyAuthenticate& src) const
{
    return  ( isEqualNoCase(authScheme, src.authScheme) &&
              myParamList == src.myParamList);
}


void
SipProxyAuthenticate::decode(const Data& data)
{
    Data nData = data;

    try
    {
        scanSipProxyauthorization(nData);
    }
    catch (SipProxyAuthenticateParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipProxyAuthenticateParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__,
                DECODE_FAILED_PROXYAUTHENTICATE
            );
        }
    }
}


void
SipProxyAuthenticate::scanSipProxyauthorization(const Data& tmpdata)
{
    Data authdetails = tmpdata ;
    Data authType;
    int ret = authdetails.match(" ", &authType, true);
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipProxyAuthenticateParserException(
                "failed to decode the ProxyAuthenticate string",
                __FILE__,
                __LINE__,
                DECODE_FAILED_PROXYAUTHENTICATE);
        }

    }
    else if (ret == FOUND)
    {
        setAuthScheme(authType);
        cpLog(LOG_DEBUG_STACK,"Found aunth sceme [%s]", authType.logData());

        if ( isEqualNoCase(authType, AUTH_BASIC ) ||
             isEqualNoCase(authType, AUTH_DIGEST ) ||
             isEqualNoCase(authType, AUTH_PGP ) )
        {
            try
            {
                myParamList.decode(authdetails, ',');
            }
            catch (SipProxyAuthenticateParserException&)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw (SipProxyAuthenticateParserException(
                        "failed in parsing auth tokens",
                        __FILE__,
                                   __LINE__,
                        DECODE_FAILED_PROXYAUTHENTICATE));
                }
            }
        }
        
    }
    else if (ret == NOT_FOUND)
    {
        throw SipProxyAuthenticateParserException(
            "No AuthScheme",
            __FILE__,
            __LINE__,
            DECODE_FAILED_PROXYAUTHENTICATE);
    }

}



/*** return the encoded string version of this. This call should only be
     used inside the stack and is not part of the API */
Data
SipProxyAuthenticate::encode() const
{
    Data data;

    if ( authScheme.length() )
    {
        data += SIP_PROXYAUTHENTICATE;
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
SipProxyAuthenticate::setAuthTokenDetails(const Data& token,
        const Data& tokenValue)
{
    myParamList.setValue(token, tokenValue);
}


///
void
SipProxyAuthenticate::setRealmValue(const Data& data)
{
    myParamList.setValue(REALM, data);
}


///
Data
SipProxyAuthenticate::getTokenValue(const Data& token) const
{
    Data ret;
    //Strip off any "" from the value
    string tokenstr = myParamList.getValue(token).convertString();
    int pos;
    pos = tokenstr.find("\"");

    if (pos != static_cast<int>(string::npos))
    {
         tokenstr = tokenstr.substr(pos + 1, tokenstr.length() - 2);                }

    ret = tokenstr;

    return ret;
}


///
Data
SipProxyAuthenticate::getRealmValue() const
{
    return getTokenValue(REALM);
}



SipHeader*
SipProxyAuthenticate::duplicate() const
{
    return new SipProxyAuthenticate(*this);
}


bool
SipProxyAuthenticate::compareSipHeader(SipHeader* msg) const
{
    SipProxyAuthenticate* otherMsg = dynamic_cast<SipProxyAuthenticate*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
