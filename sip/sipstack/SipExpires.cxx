
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

static const char* const SipExpires_cxx_Version =
    "$Id: SipExpires.cxx,v 1.2 2004/06/16 06:51:25 greear Exp $";


#include "global.h"
#include "SipExpires.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipExpiresParserException::getName( void ) const
{
    return "SipExpiresParserException";
}


SipExpires::SipExpires( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip),
      delta("")
{
    if (srcData == "") {
        return;
    }
    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipExpiresParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in EXPIRE Constructor :( ");
            throw SipExpiresParserException(
                "failed to decode the EXPIRE string",
                __FILE__,
                __LINE__, DECODE_EXPIRE_FAILED);
        }

    }

}
void SipExpires::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipExpires(nData);
    }
    catch (SipExpiresParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in EXPIRE Constructor :( ");
            throw SipExpiresParserException(
                "failed to decode the EXPIRE string",
                __FILE__,
                __LINE__, DECODE_EXPIRE_FAILED);
        }

    }

}

void
SipExpires::scanSipExpires(const Data &tmpdata)

{
    for(int i=0; i < tmpdata.length(); i++)
    {
        if(!isdigit(tmpdata.getChar(i)))
        {
            setDelta("3600");
            return;
        }        
    }
    setDelta(tmpdata);
}




SipExpires::SipExpires( const SipExpires& src )
    : SipHeader(src),
      delta(src.delta)
{}


bool
SipExpires::operator ==(const SipExpires& src) const
{
    return delta == src.delta;
}

bool
SipExpires::operator <( const SipExpires& src ) const
{
    return delta < src.delta;
}

const SipExpires&
SipExpires::operator=( const SipExpires& src )
{
    if ( &src != this )
    {
        delta = src.delta;
    }
    return *this;
}

void SipExpires::setDelta ( const Data& data)
{
    delta = data;
    cpLog(LOG_DEBUG_STACK, "set delta in expires");
}

Data SipExpires::getDelta() const
{
    return delta;
}

///
Data SipExpires::encode() const
{
    Data data;    
    if (delta != "")
    {
        data = SIP_EXPIRES;
        data += SP;
        data += getData();
	data += CRLF;
    }
    return data;
}

Data SipExpires::getData() const
{
    return delta;
}



Sptr<SipHeader>
SipExpires::duplicate() const
{
    return new SipExpires(*this);
}


bool
SipExpires::compareSipHeader(SipHeader* msg) const
{
    SipExpires* otherMsg = dynamic_cast<SipExpires*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
