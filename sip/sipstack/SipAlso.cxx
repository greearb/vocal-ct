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

static const char* const SipAlso_cxx_Version =
    "$Id: SipAlso.cxx,v 1.3 2004/06/16 06:51:25 greear Exp $";


#include "global.h"
#include "SipAlso.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "BaseUrl.hxx"
#include "Sptr.hxx"
#include "SipUrl.hxx"
    
using namespace Vocal;

string
SipAlsoParserException::getName( void ) const
{
    return "SipAlsoParserException";
}

SipAlso::SipAlso(const SipAlso& src)
    : SipHeader(src),
    alsoUrl(duplicateUrl(src.alsoUrl)),
    urlType(src.urlType),
    displayName(src.displayName)
{
}


SipAlso::SipAlso( Sptr <BaseUrl>  url, const string& _local_ip)
    : SipHeader(_local_ip),
    alsoUrl(duplicateUrl(url)),
    displayName()
{
}


SipAlso::SipAlso( const Data& data, const string& _local_ip, UrlType uType )
    : SipHeader(_local_ip),
        alsoUrl(),
        displayName()
{
    urlType = uType;

    if (data == "") {        
        return;
    }

    try
    {
        decode(data);
	if (alsoUrl.getPtr() != 0)
	{
	    urlType = alsoUrl->getType();
	}
    }
    catch (SipAlsoParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, DECODE_ALSO_FAILED);

        }

    }
}



void
SipAlso::scanSipAlso( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        Data value;
        Data tmpval;
        int retn = data.match("\n", &value, true) ;
        if (retn == NOT_FOUND)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, DECODE_ALSO_FAILED);

            }

        }
        else if (retn == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, DECODE_ALSO_FAILED);

            }

        }
        else if (retn == FOUND)
        {
            alsoUrl = BaseUrl::decode(value, local_ip);
        }

    }
    else if (ret == FIRST)
    {
        try
        {
            parseUrl(data);
        }
        catch (SipAlsoParserException&)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Scan SIPTO :o( ");
                throw SipAlsoParserException("failed in Decode in scan SipAlso", __FILE__, __LINE__, PARSE_AURL_FAILED);

            }

        }

    }

    else if (ret == FOUND)
    {
        parseNameInfo(sipdata);
        Data urlvalue;
        parseUrl(data);
    }
}




void
SipAlso::parseUrl(const Data& data)
{
    Data urlvalue = data;
    Data avalue;
    int retur = urlvalue.match(">", &avalue, true);
    if (retur == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl() :o( ");
            throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, PARSE_AURL_FAILED);

        }


    }
    else if (retur == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl()  :o( ");
            throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, PARSE_AURL_FAILED);

        }

    }
    else if (retur == FOUND)
    {
        alsoUrl = BaseUrl::decode(avalue, local_ip);
    }
}






void
SipAlso::parseNameInfo(const Data& data)
{
    Data newnameinfo;
    Data nameinfo = data;
    int ret = nameinfo.match(":", &newnameinfo, true);
    Data newnameinf;
    ret = nameinfo.match(" ", &newnameinf, true);
    setDisplayName(nameinfo);
}







void
SipAlso::parse( const Data& alsodata)
{

    Data data = alsodata;

    try
    {
        scanSipAlso(data);
    }
    catch (SipAlsoParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");
            throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, DECODE_ALSO_FAILED);

        }

    }


    //everything allright.


}


void
SipAlso::decode( const Data& alsostr )
{

    try
    {
        parse(alsostr);
    }
    catch (SipAlsoParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");
            throw SipAlsoParserException("failed in Decode", __FILE__, __LINE__, DECODE_ALSO_FAILED);

        }



    }
}


SipAlso& SipAlso::operator = (const SipAlso& srcAlso)
{
    if ( &srcAlso != this )
    {
	alsoUrl = duplicateUrl(srcAlso.alsoUrl);
	urlType = srcAlso.urlType;
        displayName = srcAlso.displayName;
    }
    return (*this);
}


bool SipAlso::operator ==(const SipAlso& srcAlso) const
{
    cpLog(LOG_DEBUG_STACK, "SipAlso ::operator ==");

    bool equal = false;
    if (alsoUrl->areEqual(srcAlso.alsoUrl))
    {
        equal = true;
    }
    if (equal == false)
    {
        cpLog(LOG_DEBUG_STACK, " url match failed" );

	if (getUrl().getPtr() != 0)
	{
	    cpLog(LOG_DEBUG_STACK, "this.url = %s" , getUrl()->encode().logData());
	}
#ifdef __linux__
	if (srcAlso.getUrl().getPtr() != 0)
	{
	    cpLog(LOG_DEBUG_STACK, "other.url = %s" ,
		  srcAlso.getUrl()->encode().logData() );
	}
#endif
    }
    return equal;
}

void SipAlso::setDisplayName(const Data& name)
{
    displayName = name;
}

Data SipAlso::getDisplayName() const
{
    return displayName;
}

void SipAlso::setHost(const Data& newhost)
{
    if (urlType == TEL_URL)
    {
	return;
    }
    if (alsoUrl.getPtr() == 0)
    {
	alsoUrl = new SipUrl("", local_ip);
    }
    if (alsoUrl.getPtr() != 0)
    {
    	if (alsoUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl((SipUrl*)(alsoUrl.getPtr()));
	    sipUrl->setHost(newhost);
	}
    } 
}

Data SipAlso::getHost() const
{
    Data host;
    if (alsoUrl.getPtr() != 0)
    {
    	if (alsoUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl((SipUrl*)(alsoUrl.getPtr()));
	    host = sipUrl->getHost();
	}
    } 
    return host;
}


Data SipAlso::encode() const
{
    Data sipalso;

    sipalso = "Also:";
    sipalso += SP;

    if (displayName.length() > 0)
    {
        sipalso += displayName;
    }

    if (alsoUrl.getPtr() != 0)
    {
	alsoUrl->encode();
	    
	//get before the url param.
	Data nameaddr;
	
	if (alsoUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl((SipUrl*)(alsoUrl.getPtr()));
	    
	    sipalso += "<";
	    
	    nameaddr = sipUrl->getNameAddr();
	    
	    Data test = nameaddr;
	    
	    sipalso += nameaddr;
	    
	    Data userparam;
	    userparam = sipUrl->getUrlParam();
	    
	    
	    if (userparam.length())
	    {
		sipalso += userparam;
	    }
	    
	    sipalso += ">";
	}
    }
	
    sipalso += CRLF;
    
    return sipalso;
}
   
    
///
Sptr <BaseUrl> 
SipAlso::getUrl() const
{
    return duplicateUrl(alsoUrl, true);
}

///
void SipAlso::setUrl(Sptr <BaseUrl> alsourl)
{
    alsoUrl = duplicateUrl(alsourl);
}


Sptr<SipHeader>
SipAlso::duplicate() const
{
    return new SipAlso(*this);
}


bool
SipAlso::compareSipHeader(SipHeader* msg) const
{
    SipAlso* otherMsg = dynamic_cast<SipAlso*>(msg);
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
