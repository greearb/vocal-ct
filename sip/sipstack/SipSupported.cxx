
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

static const char* const SipSupported_cxx_Version =
    "$Id: SipSupported.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "global.h"
#include "SipSupported.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipSupportedParserException::getName( void ) const
{
    return "SipSupportedParserException";
}


SipSupported::SipSupported( const SipSupported & src)
    : SipHeader(src)
{
    data = src.data;
}

SipSupported& SipSupported::operator=(const SipSupported& src)
{
    if ( &src != this)
    {
        data = src.data;
    }
    return (*this);
}

SipSupported::SipSupported( const Data& srcData, const string& local_ip )
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
    catch (SipSupportedParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of Supported :( ");
            throw SipSupportedParserException(
                "failed to decode the Supported string :(",
                __FILE__,
                __LINE__, DECODE_SUPPORTED_FAILED);
        }


    }

}
void SipSupported::decode(const Data& idata)
{

    Data nData = idata;

    try
    {

        scanSipSupported(nData);
    }
    catch (SipSupportedParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of Supported :( ");
            throw SipSupportedParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_SUPPORTED_FAILED
            );
        }
    }
}

void
SipSupported::scanSipSupported(const Data &tmpdata)
{
    set(tmpdata);
}



Data SipSupported::get()
{
    return data;
}

void SipSupported::set( const Data& newdata)
{
    data = newdata;
}

Data SipSupported::encode() const
{
    Data ret;
    if (data.length())
    {
        ret += data;
    }
    return ret;
}


bool SipSupported::operator ==(const SipSupported& src) const
{
    if (data == src.data)
    {
        return true;
    }  
    else
    {
        return false;
    }
}



SipHeader*
SipSupported::duplicate() const
{
    return new SipSupported(*this);
}


bool
SipSupported::compareSipHeader(SipHeader* msg) const
{
    SipSupported* otherMsg = dynamic_cast<SipSupported*>(msg);
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
