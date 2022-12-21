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
#include "SipHide.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipHideParserException::getName( void ) const
{
    return "SipHideParserException";
}

///

SipHide::SipHide( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip)
{
    if (srcData == "") {
        return;
    }
    try
    {
        decode(srcData);
    }
    catch (SipHideParserException&)
    {


        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor : ( ");
            throw SipHideParserException("failed in Decode", __FILE__, __LINE__, DECODE_HIDE_FAILED);

        }

    }
}

const SipHide&
SipHide::operator=(const SipHide& src)
{
    if (&src != this)
    {
        data = src.data;
    }
    return (*this);
}

bool
SipHide::operator==(const SipHide& src) const
{
    return     ( data == src.data );
}

void SipHide::decode(const Data& vdata)
{

    Data data1 = vdata;

    try
    {

        scanHide(data1);
    }
    catch (SipHideParserException e)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() of Hide : ( ");
            throw SipHideParserException(
                e.getDescription(),
                __FILE__,
                __LINE__, DECODE_HIDE_FAILED
            );
        }
    }

}
void
SipHide::scanHide(const Data & scandata)

{
    Data aedata = scandata;
    Data aevalue;
    if (aedata == HIDE_ROUTE)
    {
        set(aedata);
    }
    if (aedata == HIDE_HOP)
    {
        set(aedata);
    }
    else
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Value out of Range in HIDE it Should be Route|Hop : ( ");
            throw SipHideParserException("failed in Decode", __FILE__, __LINE__, DECODE_HIDE_FAILED);

        }
    }
}

///
SipHide::SipHide( const SipHide& src )
    : SipHeader(src),
      data(src.data)
{}


///

Data SipHide::get() const
{
    return data;
}

///
void SipHide::set( const Data& newData)
{
    data = newData;

}

///
Data SipHide::encode() const
{
    Data ret;
    if (data.length())
    {
        ret = HIDE;
        ret += SP;
        ret += data;
        ret += CRLF;
    }
    return ret ;
}


Sptr<SipHeader>
SipHide::duplicate() const
{
    return new SipHide(*this);
}


bool
SipHide::compareSipHeader(SipHeader* msg) const
{
    SipHide* otherMsg = dynamic_cast<SipHide*>(msg);
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
