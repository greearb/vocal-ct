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

static const char* const SipReplaces_cxx_version =
    "$Id: SipReplaces.cxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "SipParserMode.hxx"
#include "SipReplaces.hxx"
#include "symbols.hxx"

using namespace Vocal;


SipReplaces::SipReplaces(const SipReplaces& src)
    : SipHeader(src),
      callId(src.callId),
      params(src.params)
{
}

SipReplaces::SipReplaces( const Data& data, const string& local_ip )
    : SipHeader(local_ip)
{
    if (data == "") {
        return;
    }

    try
    {
        decode(data);
    }
    catch (VException& e)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode Replaces header: '%s'", data.logData());
            throw SipReplacesParserException("failed to decode replaces header", __FILE__, __LINE__);
        }
    }
}

void
SipReplaces::parse( const Data & tmpdata)
{
    Data sipdata;
    Data data;
    Data sdata = tmpdata;
    int ret = sdata.match(":", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        throw SipReplacesParserException("failed in Decode", __FILE__, __LINE__);
    }
    else if (ret == FOUND) // stripped off the Replaces: header now
    {
        // !jf! should parse as a SipCallId
        int ret = sdata.match(";", &callId, true);
        if (ret == FOUND)
        {
            params.decode(sdata);
        }
        else
        {
            throw SipReplacesParserException("failed in Decode", __FILE__, __LINE__);
        }
            
        if (params.getValue("to-tag").length() == 0)
        {
            throw SipReplacesParserException("no to-tag in SipReplaces", __FILE__, __LINE__);                
        }
        if (params.getValue("from-tag").length() == 0)
        {
            throw SipReplacesParserException("no from-tag in SipReplaces", __FILE__, __LINE__);                
        }
    }
}

const Data& 
SipReplaces::getCallId() const
{
    return callId;
}

const Data 
SipReplaces::getFromTag() const
{
    return params.getValue("from-tag");
}

const Data 
SipReplaces::getToTag() const
{
    return params.getValue("to-tag");
}


void
SipReplaces::decode( const Data& data )
{
    try
    {
        Data pdata = REPLACES;
        pdata += ":";
        pdata += data;
        parse(pdata);
    }
    catch (SipReplacesParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode Replaces header: '%s'", data.logData());
            throw SipReplacesParserException("failed to decode replaces header", __FILE__, __LINE__);
        }
    }
}


SipReplaces& SipReplaces::operator = (const SipReplaces& srcReplaces)
{
    if ( &srcReplaces != this )
    {
        callId = srcReplaces.callId;
        params = srcReplaces.params;
    }
    return (*this);
}

bool SipReplaces::operator ==(const SipReplaces& srcReplaces) const
{
    // !jf! may need to do something special with from-tag and to-tag
    return srcReplaces.callId == callId && srcReplaces.params == params;
}

Data SipReplaces::encode() const
{
    Data data = REPLACES;
    data += ":";
    data += SP;
    
    data += callId;
    data += SEMICOLON;
    
    data += params.encode();

    data += CRLF;

    return data;
}

Sptr<SipHeader>
SipReplaces::duplicate() const
{
    return new SipReplaces(*this);
}

bool
SipReplaces::compareSipHeader(SipHeader* msg) const
{
    SipReplaces* otherMsg = dynamic_cast<SipReplaces*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}


