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

static const char* const SipResponseKey_cxx_Version =
    "$Id: SipResponseKey.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipResponseKey.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipResponseKeyParserException::getName( void ) const
{
    return "SipResponseKeyParserException";
}

SipResponseKey::SipResponseKey(const Data& srcData, const string& local_ip)
    : SipHeader(local_ip)
{
    if (srcData == "") {
        return;
    }
    try
    {
        decode(srcData);
    }
    catch (SipResponseKeyParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipResponseKeyParserException("failed to decode the Response Key string",
                                                __FILE__,
                                                __LINE__, DECODE_RESPONSEKEY_FAILED);
        }
    }
}
SipResponseKey::SipResponseKey(const SipResponseKey& src)
    : SipHeader(src)
{
    key = src.key;
    param = src.param;
}


///
SipResponseKey&
SipResponseKey::operator=( const SipResponseKey& src)
{
    if (&src != this)
    {
        key = src.key;
        param = src.param;
    }
    return *this;
}

///
bool
SipResponseKey::operator==( const SipResponseKey& src) const
{
  return (
        ( key == src.key ) &&
        ( param == src.param )
    );
}
///
Data SipResponseKey::getKey()
{
    return key;
}

///
void SipResponseKey::setKey( const Data& newkey )
{
    key = newkey;
}

void SipResponseKey::decode(const Data& reqdata)
{
    Data keydata = reqdata;
    Data keyscheme;
    int ret = keydata.match("SP", &keyscheme, true);
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipResponseKeyParserException(
                "failed to decode the Response Key SPace is not found between Key scheme & key param string",
                __FILE__,
                __LINE__, DECODE_RESPONSEKEY_FAILED);
        }
    }

    else if (ret == FOUND)
    {
        setKey(keyscheme);
        parseKeyParm(keydata);
    }
    else if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipResponseKeyParserException(
                "failed to decode the Response Key SPace is not found between Key scheme & key param string",
                __FILE__,
                __LINE__, DECODE_RESPONSEKEY_FAILED);
        }
    }
}
void
SipResponseKey::parseKeyParm(const Data& keyval)
{
    setParam(keyval);
}


///
Data SipResponseKey::getParam()
{
    return param;
}

///
void SipResponseKey::setParam( const Data& newparam )
{
    param = newparam;
}

///
Data SipResponseKey::encode() const
{
    Data ret;
    if ((key.length()) && (param.length()))
    {
        ret += SIP_RESPONSEKEY;
        ret += SP;
        ret += key;
        ret += "=";
        ret += param;
        ret += CRLF;
    }
    return ret;
}



SipHeader*
SipResponseKey::duplicate() const
{
    return new SipResponseKey(*this);
}


bool
SipResponseKey::compareSipHeader(SipHeader* msg) const
{
    SipResponseKey* otherMsg = dynamic_cast<SipResponseKey*>(msg);
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
