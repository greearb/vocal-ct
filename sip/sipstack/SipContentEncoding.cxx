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

static const char* const SipContentEncoding_cxx_Version =
    "$Id: SipContentEncoding.cxx,v 1.3 2004/11/19 01:54:38 greear Exp $";

#include "SipContentEncoding.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

static const char SIP_CONTENCODING[] = "Content-Encoding ";

using namespace Vocal;


string
SipContentEncodingParserException::getName( void ) const
{
    return "SipContentEncodingParserException";
}

SipContentEncoding::SipContentEncoding(const SipContentEncoding& src)
        : SipHeader(src),
          data(src.data)
{}


SipContentEncoding::SipContentEncoding( const Data& newData, const string& local_ip )
    : SipHeader(local_ip)
{
    if (newData == "") {
        return;
    }
    try
    {
        decode(newData);
    }
    catch (SipContentEncodingParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentEncodingParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTENCODING_FAILED);

        }
    }
}


const SipContentEncoding&
SipContentEncoding::operator=(const SipContentEncoding& src)
{
    if (this != &src)
    {
        data = src.data;
    }
    return *this;
}


void
SipContentEncoding::decode(const Data& indata)
{
    Data nData = indata;
    try
    {
        scanSipContentEncoding(nData);
    }
    catch (SipContentEncodingParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentEncodingParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTENCODING_FAILED);
        }
    }
}

void
SipContentEncoding::scanSipContentEncoding(const Data &tmpdata)
{
    string newdata = tmpdata.convertString() ;

    string TOKEN = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-.!%*_+'~";
    int test = newdata.find_first_not_of(TOKEN);
    if (test == -1)
    {
        set(newdata);
    }
    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to parse Token :o( ");
            throw SipContentEncodingParserException("failed in Parse'n Token ", __FILE__, __LINE__, DECODE_TOKEN_FAILED);

        }
    }
}


Data
SipContentEncoding::get()
{
    return data;
}


void
SipContentEncoding::set( const Data& newData )
{
    data = newData;
}


Data SipContentEncoding::encode() const
{
    Data ret;
    // if (data.length())
    ret += SIP_CONTENCODING ;
    ret += ":";
    ret += data ;

    return ret;
}


bool
SipContentEncoding::operator ==( const SipContentEncoding& src) const
{
    if ( data == src.data )
    {
        return true;
    }
    else
    {
        return false;
    }
}


Sptr<SipHeader>
SipContentEncoding::duplicate() const
{
    return new SipContentEncoding(*this);
}


bool
SipContentEncoding::compareSipHeader(SipHeader* msg) const
{
    SipContentEncoding* otherMsg = dynamic_cast<SipContentEncoding*>(msg);
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
