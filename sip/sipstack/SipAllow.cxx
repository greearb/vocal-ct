
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

static const char* const SipAllow_cxx_Version =
    "$Id: SipAllow.cxx,v 1.3 2004/11/19 01:54:38 greear Exp $";

#include "global.h"
#include "symbols.hxx"
#include "cpLog.h"
#include "SipAllow.hxx"
#include "SipParserMode.hxx"


using namespace Vocal;

string
SipAllowParserException::getName( void ) const
{
    return "SipAllowParserException";
}

///

SipAllow::SipAllow( const Data& srcData, const string& local_ip)
    : SipHeader(local_ip)
{
    if (srcData == "") {
        return;
    }

    Data ndata = srcData;
    try
    {
        decode(ndata);
    }
    catch (SipAllowParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipAllowParserException("failed in Decode of Allow ", __FILE__, __LINE__, DECODE_ALLOW_FAILED);

        }

    }

}

bool SipAllow::operator == (const SipAllow& src) const
{
    return ( data == src.data);
   
}


void SipAllow::decode(const Data& vdata)
{
    try
    {

        scanSipAllow(vdata);
    }
    catch (SipAllowParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Allow Constructor :o( ");
            throw SipAllowParserException("failed in ALLOW Decode", __FILE__, __LINE__, DECODE_ALLOW_FAILED);

        }
    }
}

void
SipAllow::scanSipAllow(const Data &newdata)
{
    setAllowMethod(newdata);
}


///
SipAllow::SipAllow( const SipAllow& src)
    : SipHeader(src)
{
    data = src.data;
}

///
const SipAllow&
SipAllow::operator = (const SipAllow& src)
{
    if (&src != this)
    {
        data = src.data;
    }
    return (*this);
}

///
Data SipAllow::getAllowMethod() const
{
    return data;
}

///
void SipAllow::setAllowMethod( const Data& newData )
{
    data = newData;
}

///void
Data SipAllow::encode() const
{
    Data allow;

    allow += data;
    return allow;

}



Sptr<SipHeader>
SipAllow::duplicate() const
{
    return new SipAllow(*this);
}


bool
SipAllow::compareSipHeader(SipHeader* msg) const
{
    SipAllow* otherMsg = dynamic_cast<SipAllow*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
