
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

static const char* const SipEncryption_cxx_Version =
    "$Id: SipEncryption.cxx,v 1.3 2004/11/19 01:54:38 greear Exp $";


#include "global.h"
#include "SipEncryption.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipEncryptionParserException::getName( void ) const
{
    return "SipEncryptionParserException";
}

SipEncryption::SipEncryption( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip)
{
    if (srcData == "") {
        return;
    }
    try
    {
        decode(srcData);
    }
    catch (SipEncryptionParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipEncryptionParserException(
                "failed to decode the Encryption string",
                __FILE__,
                __LINE__, DECODE_ENCRYPTION_FAILED);
        }
    }
}


void SipEncryption::decode(const Data& vdata)
{
    Data data1 = vdata;

    try
    {
        scanEncryption(data1);
    }
    catch (SipEncryptionParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipEncryptionParserException(
                "failed to decode the Encryption string",
                __FILE__,
                __LINE__, DECODE_ENCRYPTION_FAILED);
        }
    }
}

void
SipEncryption::scanEncryption(const Data & scandata)

{
    Data aedata = scandata;
    Data aevalue;
    int reth = aedata.match(" ", &aevalue, true);
    if (reth == FOUND)
    {
        setScheme(aevalue);
        Data encData = aedata;
        Data encValue;
        int ret = encData.match("=", &encValue, true);
        if (ret == FOUND)
        {
            setToken(encValue);
            setTokenValue(encData);
        }
        else if (ret == NOT_FOUND)
        {}

        else if (ret == FIRST)
        {}

    }
    else if (reth == NOT_FOUND)
    {}

    else if (reth == FIRST)
    {}

}


SipEncryption::SipEncryption( const SipEncryption& src )
    : SipHeader(src),
      scheme(src.scheme),
      token(src.token),
      tokenvalue(src.tokenvalue)
{}



bool
SipEncryption::operator ==(const SipEncryption& src) const
{
    cpLog(LOG_DEBUG_STACK, "Encryption == Operator ");


    if ( (scheme == src.scheme) &&
            (token == src.token)
       )
    {
        cpLog(LOG_DEBUG_STACK, "Encryption == Operator  returning True :)");
        return true;
    }
    else
    {
        cpLog(LOG_DEBUG_STACK, "Encryption == Operator  returning False :(");
        return false;
    }
}


const SipEncryption&
SipEncryption::operator=( const SipEncryption& src )
{
    if (this != &src)
    {
        scheme = src.scheme;
        token = src.token;
        tokenvalue = src.tokenvalue;
    }
    return *this;
}


///
Data SipEncryption::getScheme()
{
    return scheme;
}

///
void SipEncryption::setScheme( const Data& newscheme )
{
    scheme = newscheme;
}

///
Data SipEncryption::getToken()
{
    return token;
}

///
void SipEncryption::setToken( const Data& newToken )
{
    token = newToken;
}

void SipEncryption::setTokenValue( const Data& newTokenvalue )
{
    tokenvalue = newTokenvalue;
}
Data SipEncryption::getTokenValue()
{
    return tokenvalue;
}
Data SipEncryption::encode() const
{
    Data data;
    if (scheme.length())
    {
        data += ENCRYPTION;
        data += SP;
        data += scheme;
        data += SP;
        data += token;
        data += "=";
        data += tokenvalue;
        data += CRLF;
    }
    return data;
}



Sptr<SipHeader>
SipEncryption::duplicate() const
{
    return new SipEncryption(*this);
}


bool
SipEncryption::compareSipHeader(SipHeader* msg) const
{
    SipEncryption* otherMsg = dynamic_cast<SipEncryption*>(msg);
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
