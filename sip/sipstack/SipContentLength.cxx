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

static const char* const SipContentLength_cxx_Version =
    "$Id: SipContentLength.cxx,v 1.3 2004/11/19 01:54:38 greear Exp $";



#include "global.h"
#include "SipContentLength.hxx"
#include "SipMsg.hxx"
#include "symbols.hxx"

using namespace Vocal;

string
SipContentLengthParserException::getName( void ) const
{
    return "SipUrlParserException";
}


SipContentLength::SipContentLength(const SipContentLength& src)
        : SipHeader(src),
          contentLength(src.contentLength)
{}


SipContentLength::SipContentLength( const Data& data, const string& local_ip ) //TODO
    : SipHeader(local_ip)
{
    if (data == "") {
        return;
    }
    try
    {
        decode(data);
    }
    catch (SipContentLengthParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Content Length Constructor :o( ");
            throw SipContentLengthParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTLENGTH_FAILED);

        }
    }
}


void
SipContentLength::parse( const Data& cseqdata)
{
    Data data = cseqdata;
    setLengthData(cseqdata);
}



void
SipContentLength::decode( const Data& clstr )
{
    try
    {
        parse(clstr);
    }
    catch (SipContentLengthParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Content Length  :o( ");
            throw SipContentLengthParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTLENGTH_FAILED);

        }
    }
}


void SipContentLength::setLength(int length)
{
    contentLength = Data(length);
}


Data SipContentLength::encode() const
{
    Data encoding;

    encoding = CONTENT_LENGTH;
    encoding += SP;
    if ( contentLength.length() != 0)
      {
	encoding += contentLength;
      }
    else
      {
	encoding += Data("0");
      }

    encoding += CRLF;
    

    return encoding;
}


void SipContentLength::setLengthData(const Data& contentlngth)
{
    contentLength = contentlngth;
}


Data SipContentLength::getLength() const
{
    return contentLength;
}


bool SipContentLength::operator ==( const SipContentLength& src) const
{
    if (contentLength == src.contentLength)
    {
        return true;
    }
    else
    {
        return false;
    }
}


const SipContentLength&
SipContentLength::operator=(const SipContentLength& src)
{
    if (&src != this)
    {
        contentLength = src.contentLength;
    }
    return *this;
}


Sptr<SipHeader>
SipContentLength::duplicate() const
{
    return new SipContentLength(*this);
}


bool
SipContentLength::compareSipHeader(SipHeader* msg) const
{
    SipContentLength* otherMsg = dynamic_cast<SipContentLength*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}


