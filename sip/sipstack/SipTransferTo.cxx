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

static const char* const SipTransferTo_cxx_Version =
    "$Id: SipTransferTo.cxx,v 1.4 2004/11/19 01:54:38 greear Exp $";

#include "global.h"
#include "SipTransferTo.hxx"
#include "symbols.hxx"
#include "SipUrl.hxx"
#include "TelUrl.hxx"
    
using namespace Vocal;


#define NOT_FOUND -1
#define FIRST -2
#define FOUND 0

string
SipTransferToParserException::getName( void ) const
{
    return "SipTransferToParserException";
}

SipTransferTo::SipTransferTo(const SipTransferTo& src)
    : SipHeader(src),
    toUrl(duplicateUrl(src.toUrl)),
    urlType(src.urlType),
    displayName(src.displayName)
{
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(toUrl.getPtr()));
            sipUrl->initializeTo();
	}
    }
}


SipTransferTo::SipTransferTo( Sptr <BaseUrl> url, const string& local_ip )
    : SipHeader(local_ip),
        toUrl(duplicateUrl(url)),
        displayName()
{
    if (toUrl.getPtr() != 0)
    {
	urlType = toUrl->getType();
    
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(url.getPtr()));
            sipUrl->initializeTo();
	}
    }
}


SipTransferTo::SipTransferTo( const Data& data, const string& local_ip,  UrlType uType)
    : SipHeader(local_ip),
      toUrl(),
      urlType(uType),
      displayName()
{

    if (data == "") {
        return;
    }

    try
    {
        decode(data);
	if (toUrl.getPtr() != 0)
	{
	    urlType = toUrl->getType();
    
	    if (toUrl->getType() == SIP_URL)
	    {
                Sptr <SipUrl> sipUrl((SipUrl*)(toUrl.getPtr()));
                sipUrl->initializeTo();
	    }
	}
    }
    catch (SipTransferToParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TRANSFER_TO_FAILED);

        }

    }
}


void
SipTransferTo::scanSipTransferTo( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {   //it can be URL?
        // check  for the URL with Addrs-params

        if (SipParserMode::sipParserMode())
        {
            Data tmpval;
            int retn = data.match("sip", &tmpval, false);
            if (retn == NOT_FOUND)
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                    throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TRANSFER_TO_FAILED);

                }

            }
            else if (retn == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {
                    cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                    throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TRANSFER_TO_FAILED);

                }

            }
            else if (retn == FOUND)
            { // if found then it has the Addrs-params

                toUrl = BaseUrl::decode(data, getLocalIp());

            }
        }
        else
        {
            toUrl = BaseUrl::decode(data, getLocalIp());
        }
    }
    else if (ret == FIRST)
    {
        sipdata = data;
        Data sivalue;
        parseUrl(sipdata);
    }
    else if (ret == FOUND)
    {
        parseNameInfo(sipdata);
        parseUrl(data);

    }
}


void
SipTransferTo::parseUrl(const Data& data)
{
    Data urlvalue = data;
    Data avalue;
    int retur = urlvalue.match(">", &avalue, true);
    if (retur == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl() :o( ");
            throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, PARSE_TRANSFER_URL_FAILED);

        }


    }
    else if (retur == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl()  :o( ");
            throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, PARSE_TRANSFER_URL_FAILED);

        }

    }
    else if (retur == FOUND)
    {
        toUrl = BaseUrl::decode(avalue, getLocalIp());

    }
}


void
SipTransferTo::parseNameInfo(const Data& data)
{
    Data nameinfo = data;
    setDisplayName(nameinfo);
}


void
SipTransferTo::parse( const Data& todata)
{

    Data data = todata;

    try
    {

        scanSipTransferTo(data);
    }
    catch (SipTransferToParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");
            throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TRANSFER_TO_FAILED);

        }

    }
    //everything allright.
}


void
SipTransferTo::decode( const Data& tostr )
{

    try
    {
        parse(tostr);
    }
    catch (SipTransferToParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");
            throw SipTransferToParserException("failed in Decode", __FILE__, __LINE__, DECODE_TRANSFER_TO_FAILED);

        }



    }
}


SipTransferTo&
SipTransferTo::operator = (const SipTransferTo& srcTransferTo)
{
    if ( &srcTransferTo != this)
    {
        toUrl = duplicateUrl(srcTransferTo.toUrl);
	urlType = srcTransferTo.urlType;
        displayName = srcTransferTo.displayName;
    }
    return (*this);

}


bool 
SipTransferTo::operator ==(const SipTransferTo& srcTransferTo) const
{
    cpLog(LOG_DEBUG_STACK, "SipTransferTo == operator");
    bool equal = false;
    equal = (toUrl->areEqual(srcTransferTo.toUrl));

    cpLog(LOG_DEBUG_STACK, "SipTransferTo == operator returning : %d", equal);

    return equal;
}


void 
SipTransferTo::setPortData(const Data& newport)
{
    if (urlType == TEL_URL)
    {
	return;
    }
    if (toUrl.getPtr() == 0)
    {
	toUrl = new SipUrl("", getLocalIp());
    }
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
            Sptr <SipUrl> sipUrl((SipUrl*)(toUrl.getPtr()));
            sipUrl->setPort(newport);
	}
    }

}


Data
SipTransferTo::getPortData()
{
    Data port;
    
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
            Sptr <SipUrl> sipUrl((SipUrl*)(toUrl.getPtr()));
            port = sipUrl->getPort();
	}
    }
    return port;
}


int
SipTransferTo::getPort()
{
    
    int p = getPortData().convertInt();
    return p;
}


void 
SipTransferTo::setUser(const Data& newuser)
{
    if (toUrl.getPtr() == 0)
    {
	if (urlType == SIP_URL)
	{
	    toUrl = new SipUrl("", getLocalIp());
	}
	else if (urlType == TEL_URL)
	{
	    toUrl = new TelUrl();
	}
    }
    
    toUrl->setUserValue(newuser);

    //display name defaulted to user.
    if (displayName.length() == 0)
    {
        displayName = newuser;
    }
}


Data 
SipTransferTo::getUser()
{
    if(toUrl != 0)
    {
        return toUrl->getUserValue();
    }
    else
    {
        return "";
    }
}


void 
SipTransferTo::setDisplayName(const Data& name)
{
    displayName = name;
}


Data 
SipTransferTo::getDisplayName()
{
    return displayName;
}


void 
SipTransferTo::setHost(const Data& newhost)
{
    
    if (urlType == TEL_URL)
    {
	return;
    }
    if (toUrl.getPtr() == 0)
    {
	toUrl = new SipUrl("", getLocalIp());
    }
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
            Sptr <SipUrl> sipUrl((SipUrl*)(toUrl.getPtr()));
            sipUrl->setHost(newhost);
	}
    }
}

    
Data 
SipTransferTo::getHost()
{
    Data host;
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
            Sptr <SipUrl> sipUrl((SipUrl*)(toUrl.getPtr()));
            host = sipUrl->getHost();
	}
    }
    return host;
}


Data 
SipTransferTo::encode() const
{
    Data sipTransferTo;

    sipTransferTo = TRANSFER_TO;
    sipTransferTo += SP;

    if (displayName.length() > 0)
    {
        sipTransferTo += displayName;
    }

    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{	
            Sptr <SipUrl> sipUrl((SipUrl*)(toUrl.getPtr()));
    
	    sipTransferTo += "<";

	    sipUrl->encode();

	    //get before the url param.
	    Data nameaddr;
	    nameaddr = sipUrl->getNameAddr();

	    sipTransferTo += nameaddr;

	    Data userparam;
	    userparam = sipUrl->getUrlParam();

	    if (userparam.length())
	    {
		sipTransferTo += userparam;
	    }
	    sipTransferTo += ">";


	}
    }
    sipTransferTo += CRLF;
    
    return sipTransferTo;
}


Sptr <BaseUrl> 
SipTransferTo::getUrl()
{
    return duplicateUrl(toUrl, true);
}


void 
SipTransferTo::setUrl(Sptr <BaseUrl>  tourl)
{
    toUrl = duplicateUrl(tourl);
    
    if (toUrl.getPtr() != 0)
    {
	if (toUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(toUrl.getPtr()));
            sipUrl->initializeTo();
	}
    }
}


Sptr<SipHeader>
SipTransferTo::duplicate() const
{
    return new SipTransferTo(*this);
}


bool
SipTransferTo::compareSipHeader(SipHeader* msg) const
{
    SipTransferTo* otherMsg = dynamic_cast<SipTransferTo*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}


