
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

#include "global.h"
#include "SipPriority.hxx"
#include "symbols.hxx"
#include "cpLog.h"
#include "SipParserMode.hxx"

using namespace Vocal;


///
string
SipPriorityParserException::getName( void ) const
{
    return "SipPriorityParserException";
}


///
SipPriority::SipPriority( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip)
{
    if (srcData == "") {
        return;
    }
    try
    {
        decode(srcData);
    }
    catch (SipPriorityParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipPriorityParserException(
                "failed to decode the Priority string",
                __FILE__,
                __LINE__, DECODE_FAILED_PRIORITY);
        }
    }
}


///
SipPriority::SipPriority( const SipPriority& src )
    : SipHeader(src)
{}



///

SipPriority&
SipPriority::operator = (const SipPriority& src)
{
    if (&src != this)
    {
        data = src.data;
    }
    return (*this);
}
///

bool
SipPriority::operator == (const SipPriority& src) const
{
  return ( data == src.data );
}


///
void
SipPriority::decode(const Data& vdata)
{

    Data data1 = vdata;

    try
    {
        scanPriority(data1);
    }
    catch (SipPriorityParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipPriorityParserException(
                "failed to decode the Priority string",
                __FILE__,
                __LINE__, DECODE_FAILED_PRIORITY);
        }
    }
}



///
void
SipPriority::scanPriority(const Data & scandata)
{
    Data testdata = scandata;

    if ( (testdata == SIP_EMERGENCY) |
            (testdata == SIP_URGENT) |
            (testdata == SIP_NORMAL) |
            (testdata == SIP_NONURGENT))
    {
        set(testdata);
    }
    else
    {
        cpLog(LOG_DEBUG_STACK, "Sip Priority Out of Range");
    }
}


///
Data SipPriority::get() const
{
    return data;
}

///
void SipPriority::set( const Data& newdata )
{
    data = newdata;

}


///
Data SipPriority::encode() const
{
    Data ret;
    if (data.length())
    {
        ret += SIP_PRIORITY;
        ret += SP;
        ret += data;
        ret += CRLF;
    }

    return ret;
}


Sptr<SipHeader>
SipPriority::duplicate() const
{
    return new SipPriority(*this);
}


bool
SipPriority::compareSipHeader(SipHeader* msg) const
{
    SipPriority* otherMsg = dynamic_cast<SipPriority*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
