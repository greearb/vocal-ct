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

static const char* const SipProxyRequire_cxx_Version =
    "$Id: SipProxyRequire.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "global.h"
#include "symbols.hxx"
#include "SipProxyRequire.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;

static const char SIP_PROXYREQUIRE[] = "Proxy-Require";
string
SipProxyRequireParserException::getName( void ) const
{
    return "SipProxyRequireParserException";
}

///
SipProxyRequire::SipProxyRequire( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip)
{
    if (srcData == "") {
        return;
    }
    try
    {
        decode(srcData);
    }
    catch (SipProxyRequireParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipProxyRequireParserException(
                "failed to decode the Proxy Require string",
                __FILE__,
                __LINE__, DECODE_PROXYREQUIRE_FAILED);
        }
    }
}


///
SipProxyRequire::SipProxyRequire( const SipProxyRequire& src )
    : SipHeader(src),
      data(src.data)
{}

///

SipProxyRequire&
SipProxyRequire::operator=( const SipProxyRequire& src)
{
    if (&src != this)
    {
        data = src.data;
    }
    return *this;
}


bool
SipProxyRequire::operator==( const SipProxyRequire& src) const
{
  return
        (data == src.data ) ;
}

void SipProxyRequire::decode(const Data& reqdata)
{
    set(reqdata);
}
///
Data SipProxyRequire::get() const
{
    Data ret(data);
    return ret;
}

///
void SipProxyRequire::set( const Data& newdata )
{
    data = newdata;
}

///
Data SipProxyRequire::encode() const
{
    Data ret;
    ret += data;
    return ret;
}



SipHeader*
SipProxyRequire::duplicate() const
{
    return new SipProxyRequire(*this);
}


bool
SipProxyRequire::compareSipHeader(SipHeader* msg) const
{
    SipProxyRequire* otherMsg = dynamic_cast<SipProxyRequire*>(msg);
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
