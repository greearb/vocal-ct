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

static const char* const SipReferTo_cxx_version =
    "$Id: SipReferTo.cxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include "global.h"
#include "SipParserMode.hxx"
#include "SipReferTo.hxx"
#include "symbols.hxx"
#include "SipUrl.hxx"
#include "BaseUrl.hxx"
#include "Sptr.hxx"

using namespace Vocal;

string
SipReferToParserException::getName( void ) const
{
    return "SipReferToParserException";
}

SipReferTo::SipReferTo(const SipReferTo& src)
    : SipHeader(src),
        urlType(src.urlType),
        address(src.address),
        displayName(src.displayName),
        afterAngleQuote(src.afterAngleQuote),
        angleQuotes(src.angleQuotes)
{
}


SipReferTo::SipReferTo( const Data& data, const string& local_ip )
    : SipHeader(local_ip),
        urlType(),
        address(),
        displayName(),
        afterAngleQuote(),
        angleQuotes(false)
{

    if (data == "") {
        return;
    }

    decode(data);
}


SipReferTo::SipReferTo(Sptr <BaseUrl> url, const string& local_ip)
    : SipHeader(local_ip)
{
    if (url.getPtr() != 0)
    {
        if (url->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(url.getPtr()));
	    Data x = sipUrl->getNameAddr();
	    Data sipdata;
	    int ret = x.match(":", &sipdata, true);
	    if ( ret == FOUND)
	    {
		urlType = sipdata;
		address = x;
	    }
	}
    }
}

void
SipReferTo::scanSipReferTo( const Data & tmpdata)
{
    Data sipdata;
    Data data;
    Data sdata = tmpdata;
    int ret;
    ret = sdata.match("<", &displayName, true);
    
    if (ret == FOUND || ret == FIRST)
    {
        angleQuotes = true;
    }
    ret = sdata.match(":", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipReferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERTO_FAILED);
        }
    }
    else if (ret == FOUND)
    {
        Data urldata;
        urlType = sipdata;
        int retn = sdata.match("//", &urldata, true);
        if (retn == FIRST)
        {
            Data urData;
            if (angleQuotes)
            {
                if (sdata.match(">", &address, true) == FOUND)
                {
                    if (sdata.match(CRLF, &urData, true) == FOUND)
                    {
                        afterAngleQuote = urData;
                    }
                    else
                    {
                        afterAngleQuote = sdata;
                    }
                }
                else
                {
                    throw SipReferToParserException("failed in Decode in scan SipAlso", __FILE__, __LINE__, PARSE_REFERURL_FAILED);
                }
            }
            else
            {
                int retl = sdata.match(CRLF, &urData, true);
                if ( retl == FOUND)
                {
                    address = urData;
                }
                else if ( retl == NOT_FOUND)
                {
                    address = sdata;
                }
            }
        }
        else
        {
            sipdata.removeSpaces();
            if ( sipdata == "sip")
            {
                Data urData;
                if (angleQuotes)
                {
                    if (sdata.match(">", &address, true) == FOUND)
                    {
                        if (sdata.match(CRLF, &urData, true) == FOUND)
                        {
                            afterAngleQuote = urData;
                        }
                        else
                        {
                            afterAngleQuote = sdata;
                        }
                    }
                    else
                    {
                        throw SipReferToParserException("failed in Decode in scan SipAlso", __FILE__, __LINE__, PARSE_REFERURL_FAILED);
                    }
                }
                else
                {
                    int retl = sdata.match(CRLF, &urData, true);
                    if ( retl == FOUND)
                    {
                        address = urData;
                    }
                    else if ( retl == NOT_FOUND)
                    {
                        address = sdata;
                    }
                }
            }
            else
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Failed to Decode in Scan SIPTO :o( ");
                    throw SipReferToParserException("failed in Decode in scan SipAlso", __FILE__, __LINE__, PARSE_REFERURL_FAILED);
                }
            }
        }
    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Scan SIPTO :o( ");
            throw SipReferToParserException("failed in Decode in scan SipAlso", __FILE__, __LINE__, PARSE_REFERURL_FAILED);
        }
    }
}


void
SipReferTo::parse( const Data& refertodata)
{
    Data data = refertodata;
    scanSipReferTo(data);
/*
    try
    {
        scanSipReferTo(data);
    }
    catch (SipReferToParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");
            throw SipReferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERTO_FAILED);
        }
    }
*/
}

void
SipReferTo::decode( const Data& alsostr )
{
    parse(alsostr);
/*
    try
    {
        parse(alsostr);
    }
    catch (SipReferToParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");
            throw SipReferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_REFERTO_FAILED);
        }
    }
*/
}


SipReferTo& SipReferTo::operator = (const SipReferTo& srcReferTo)
{
    if ( &srcReferTo != this )
    {
        urlType = srcReferTo.urlType;
        address = srcReferTo.address;
        displayName = srcReferTo.displayName;
        afterAngleQuote = srcReferTo.afterAngleQuote;
        angleQuotes = srcReferTo.angleQuotes;
    }
    return (*this);
}


bool SipReferTo::operator ==(const SipReferTo& srcReferTo) const
{
    cpLog(LOG_DEBUG_STACK, "SipAlso ::operator ==");

    bool equal = false;
    if (urlType == srcReferTo.urlType)
    {
        equal = true;

        if ( address != srcReferTo.address)
        {
            equal = false;
        }
        
        if ( afterAngleQuote != srcReferTo.afterAngleQuote)
        {
            equal = false;
        }
    }
    if (equal == false)
    {
        cpLog(LOG_DEBUG_STACK, " url match failed" );

    }
    return equal;
}

Data SipReferTo::getUrlString() const
{
    if (( urlType.length() == 0) && ( address.length() == 0))
    {
        Data temp;
        return temp;
    }
    Data sipalso;

    sipalso += urlType;
    if ( urlType == "sip")
    {
        sipalso += ":";
    }
    else
    {
        sipalso += "://";
    }
    sipalso += address;
    return sipalso;
}    


Data SipReferTo::encode() const
{
    if (( urlType.length() == 0) && ( address.length() == 0))
    {
        Data temp;
        return temp;
    }
    Data sipalso;

    sipalso = REFER_TO;
    sipalso += ":";
    sipalso += SP;
    
    if (angleQuotes)
    {
        sipalso += displayName;
        sipalso += "<";
    }

    sipalso += urlType;
    if ( urlType == "sip")
    {
        sipalso += ":";
    }
    else
    {
        sipalso += "://";
    }
    sipalso += address;
    if (angleQuotes)
    {
        sipalso += ">";
        sipalso += afterAngleQuote;
    }
    
    sipalso += CRLF;

    return sipalso;
}


///

Data SipReferTo::getUrlType() const
{
    return urlType;
}








SipHeader*
SipReferTo::duplicate() const
{
    return new SipReferTo(*this);
}


bool
SipReferTo::compareSipHeader(SipHeader* msg) const
{
    SipReferTo* otherMsg = dynamic_cast<SipReferTo*>(msg);
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
