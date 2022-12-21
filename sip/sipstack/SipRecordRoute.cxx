
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
#include "SipRecordRoute.hxx"
#include "SipParserMode.hxx"
#include "SipUrl.hxx"
#include "symbols.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipRecordRouteParserException::getName( void ) const
{
    return "SipRecordRouteParserException";
}

SipRecordRoute::SipRecordRoute( const SipRecordRoute& src )
    : SipHeader(src),
      url(duplicateUrl(src.url)),
      urlType(src.urlType),
      displayname(src.displayname)
{
}


SipRecordRoute::SipRecordRoute(const Data& newData, const string& local_ip,
                               UrlType uType)
    : SipHeader(local_ip),
      url()
{

    if (newData == "") {
        urlType = uType;
        return;
    }

    try
    {
        decode(newData);
	if (url.getPtr() != 0)
	{
	    urlType = url->getType();
	}
    }
    catch (SipRecordRouteParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of Record ROute :( ");
            throw SipRecordRouteParserException(
                "failed to decode the Record-Route string :(",
                __FILE__,
                __LINE__, DECODE_RECORDROUTE_FAILED);
        }
    }
}


void
SipRecordRoute::decode( const Data& rrstr )
{

    try
    {
        parse(rrstr);
    }
    catch (SipRecordRouteParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() of Record ROute :( ");
            throw SipRecordRouteParserException(
                "failed to decode the Record-Route string :(",
                __FILE__,
                __LINE__, DECODE_RECORDROUTE_FAILED);
        }

    }
}


void
SipRecordRoute::parse( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse() of Record ROute :( ");
            throw SipRecordRouteParserException(
                "failed to decode the Record-Route string :(",
                __FILE__,
                __LINE__, DECODE_RECORDROUTE_FAILED);
        }
    }
    else if (ret == FIRST)
    {
        // which is fine
        parseUrl(data);
    }
    else if (ret == FOUND)
    {
        // this is also fine becos name-addrs :dispaly-name <addr-spec>
        setDisplayName(sipdata);
        parseUrl(data);
    }
}
void
SipRecordRoute::parseUrl(const Data & tmpurl)
{
    Data gdata = tmpurl;
    Data urlvalue;
    int retn = gdata.match(">", &urlvalue, true);
    if (retn == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in ParseUrl() of Record ROute :( ");
            throw SipRecordRouteParserException(
                "failed to decode the Record-Route string :(",
                __FILE__,
                __LINE__, URL_PARSE_FAIL);
        }
    }
    else if (retn == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in ParseUrl() of Record ROute :( ");
            throw SipRecordRouteParserException(
                "failed to decode the Record-Route string :(",
                __FILE__,
                __LINE__, URL_PARSE_FAIL);
        }

    }
    else if (retn == FOUND)
    {

        url = BaseUrl::decode(urlvalue, getLocalIp());

    }

}


Sptr <BaseUrl> 
SipRecordRoute::getUrl() const
{
    return duplicateUrl(url,true);
}


void
SipRecordRoute::setUrl( Sptr <BaseUrl> newUrl )
{
    url = duplicateUrl(newUrl);
}

    
Data
SipRecordRoute::encode() const
{
    Data data;
    Data disname = getDisplayName();
    if (disname.length() > 0)
    {
        data += disname;
    }
    
    if (url.getPtr() != 0)
    {
	if (url->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(url.getPtr()));
    
	    data += "<";
	    sipUrl->encode();
	    Data name = sipUrl->getNameAddr();
	    data += name;

#if 1
	    Data transportParam = sipUrl->getTransportParam();
            if ((transportParam.length() > 0) &&
                (transportParam == Data("tcp")))
            {
                data += SEMICOLON;
                data += SipUrlParamTransport;
                data += transportParam;
            }
#endif
    
	    Data recRouteMaddr = sipUrl->getMaddrParam();
	    if ( recRouteMaddr.length() > 0)
	    {
		data += ";";
		data += "maddr=";
		data += recRouteMaddr;
	    }
    
            if (sipUrl->isLooseRouterPresent())
            {
                data += ";lr";
            }
            
	    data += ">";
	}
    }

    return data;
}


const SipRecordRoute& 
SipRecordRoute::operator=(const SipRecordRoute& src)
{
    if ( &src != this )
    {
	url = duplicateUrl(src.url);
        displayname = src.displayname;
    }
    return *this;
}


bool 
SipRecordRoute::operator == (const SipRecordRoute& src) const
{
    bool equal = false;
    
    if ( (url.getPtr() != 0) && (src.url.getPtr() != 0) )
    {
	equal = ( url->areEqual(src.url));
    }
    else if ( (url.getPtr() == 0) && (src.url.getPtr() == 0) )
    {
	equal = true;
    }
    else
    {
	equal = false;
    }
    equal = (equal && 
	     (displayname == src.displayname)
	);
    return equal;
}


Sptr<SipHeader>
SipRecordRoute::duplicate() const
{
    return new SipRecordRoute(*this);
}


bool
SipRecordRoute::compareSipHeader(SipHeader* msg) const
{
    SipRecordRoute* otherMsg = dynamic_cast<SipRecordRoute*>(msg);
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
