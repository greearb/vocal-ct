
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

static const char* const SipInReplyTo_cxx_Version =
    "$Id: SipInReplyTo.cxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "global.h"
#include "symbols.hxx"
#include "SipInReplyTo.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipInReplyToParserException::getName( void ) const
{
    return "SipInReplyToParserException";
}

SipInReplyTo&
SipInReplyTo::operator=(const SipInReplyTo& src)
{
    if ( &src != this )
    {
        inreplyto = src.inreplyto;
        calliddata = src.calliddata;
    }
    return (*this);
}

///
/// 
bool SipInReplyTo::operator ==(const SipInReplyTo & src) const
{
  return (
	  ( inreplyto == src.inreplyto ) &&
	  ( calliddata == src.calliddata ) 
	  );
}
///
SipInReplyTo::SipInReplyTo( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip),
      inreplyto("", local_ip)
{
    if (srcData == "") {
        return;
    }
    Data ndata = srcData;
    try
    {
        decode(ndata);
    }
    catch (SipInReplyToParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");
            throw SipInReplyToParserException("failed in Decode", __FILE__, __LINE__, DECODE_INREPLYTO_FAILED);

        }

    }

}
void SipInReplyTo::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipInReplyTo(nData);
    }
    catch (SipInReplyToParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor in Sip InReply TO  : ( ");
            throw SipInReplyToParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_INREPLYTO_FAILED
            );
        }
    }

}

void
SipInReplyTo::scanSipInReplyTo(const Data &tmpdata)
{
    SipCallId newid(tmpdata, getLocalIp());
    inreplyto = newid;
}


///
SipInReplyTo::SipInReplyTo( const SipInReplyTo& src )
    : SipHeader(src),
      inreplyto(src.inreplyto),
      calliddata(src.calliddata)
{}

///

Data SipInReplyTo::get() const
{
    calliddata = inreplyto.getCallIdData();
    return calliddata;
}

///
void SipInReplyTo::set( const Data& newdata )
{
    calliddata = newdata;
    inreplyto.setData(calliddata);
}

///
Data SipInReplyTo::encode() const
{
    Data data;
    Data ldata = get(); 
    if (ldata.length())
    {
        data += "In-Reply-To:";
        data += SP;
        data += ldata;
        data += CRLF;
    }

    return data;
}



Sptr<SipHeader>
SipInReplyTo::duplicate() const
{
    return new SipInReplyTo(*this);
}


bool
SipInReplyTo::compareSipHeader(SipHeader* msg) const
{
    SipInReplyTo* otherMsg = dynamic_cast<SipInReplyTo*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}
