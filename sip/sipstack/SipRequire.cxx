
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

static const char* const SipRequire_cxx_Version =
    "$Id: SipRequire.cxx,v 1.3 2004/11/19 01:54:38 greear Exp $";

#include "global.h"
#include "SipRequire.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;

string
SipRequireParserException::getName( void ) const
{
    return "SipRequireParserException :o(";
}


///
SipRequire::SipRequire( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip)
{
    if (srcData == "") {
        return;
    }

    Data wdata = srcData;
    try
    {
        decode(wdata);
    }
    catch (SipRequireParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");
            throw SipRequireParserException("failed in Decode", __FILE__, __LINE__, DECODE_REQUIRE_FAILED);

        }

    }

}

///
SipRequire&
SipRequire::operator=( const SipRequire& src )
{
    if (&src != this)
    {
        data = src.data;
    }
    return *this;
}

///
bool
SipRequire::operator==( const SipRequire& src ) const
{
    return ( data == src.data );
}

void
SipRequire::decode(const Data& ndata)
{

    Data nData = ndata;

    try
    {

        scanSipRequire(nData);
    }
    catch (SipRequireParserException e)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");
            throw SipRequireParserException(
                e.getDescription(),
                __FILE__,
                __LINE__, DECODE_REQUIRE_FAILED
            );
        }
    }

}

void
SipRequire::scanSipRequire(const Data &tmpdata)
{
    set(tmpdata);
}

///
SipRequire::SipRequire( const SipRequire& src )
    : SipHeader(src),
      data(src.data)
{}

///

Data SipRequire::get()
{
    return data;
}

///
void SipRequire::set( const Data& newdata )
{
    data = newdata;
}

///
Data SipRequire::encode() const
{
    Data ret;
    ret += data;
    return ret;



}



Sptr<SipHeader>
SipRequire::duplicate() const
{
    return new SipRequire(*this);
}


bool
SipRequire::compareSipHeader(SipHeader* msg) const
{
    SipRequire* otherMsg = dynamic_cast<SipRequire*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
