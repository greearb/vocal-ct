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

static const char* const SipRoute_cxx_Version =
    "$Id: SipRoute.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "global.h"
#include "SipRecordRoute.hxx"
#include "SipRoute.hxx"
#include "SipParserMode.hxx"
#include "SipUrl.hxx"

using namespace Vocal;


string
SipRouteParserException::getName( void ) const
{
    return "SipRouteParserException";
}



SipRoute::SipRoute( const Data& srcData, const string& local_ip,  UrlType uType)
    : SipHeader(local_ip),
      url(),
      urlType(uType)
{
    if (srcData == "") {
        return;
    }

    try
    {
        decode(srcData);
	if (url != 0)
	{
	    urlType = url->getType();
	}
    }
    catch (SipRouteParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of ROute :( ");
            throw SipRouteParserException(
                "failed to decode the Route string :(",
                __FILE__,
                __LINE__, DECODE_ROUTE_FAILED);
        }


    }
}


SipRoute::SipRoute( const SipRoute& src )
    : SipHeader(src),
      url(duplicateUrl(src.url)), 
      urlType(src.urlType),
      displayname(src.displayname)
{
}

SipRoute::SipRoute( const SipRecordRoute& rr)
    : SipHeader(rr.getLocalIp()), 
      url(duplicateUrl(rr.url)),
      urlType(rr.urlType),
      displayname(rr.displayname)
{
}


void
SipRoute::decode( const Data& rrstr )
{

    try
    {
        parse(rrstr);
    }
    catch (SipRouteParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of ROute :( ");
            throw SipRouteParserException(
                "failed to decode the Route string :(",
                __FILE__,
                __LINE__, DECODE_ROUTE_FAILED);
        }

    }
}


void
SipRoute::parse( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse() of Record ROute :( ");
            throw SipRouteParserException(
                "failed to parse the Record-Route string :(",
                __FILE__,
                __LINE__, DECODE_ROUTE_FAILED);
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
SipRoute::parseUrl(const Data & tmpurl)
{
    Data gdata = tmpurl;
    Data urlvalue;
    int retn = gdata.match(">", &urlvalue, true);
    if (retn == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in ParseUrl() of  ROute :( ");
            throw SipRouteParserException(
                "failed to decode the Route string :(",
                __FILE__,
                __LINE__, ROUTE_URL_PARSE_FAIL);
        }
        //Exceptions
    }

    else if (retn == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in ParseUrl() of  ROute :( ");
            throw SipRouteParserException(
                "failed to decode the Route string :(",
                __FILE__,
                __LINE__, ROUTE_URL_PARSE_FAIL);
        }
        //EXCEPTions
    }

    else if (retn == FOUND)
    {

        url = BaseUrl::decode(urlvalue, getLocalIp());
    }

}


Sptr <BaseUrl> SipRoute::getUrl() const
{
    return duplicateUrl(url, true);
}


void SipRoute::setUrl( Sptr <BaseUrl> srcurl )
{
    url = duplicateUrl(srcurl);
}


Data SipRoute::encode() const
{

    Data data;
    if (url.getPtr() != 0)
    {
	if (url->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(url);
    
	    Data urlData = sipUrl->getNameAddr();

	    if ( (displayname.length()) || (urlData.length()) )
	    {
		data += displayname;
		
		data = "<";
		data += urlData;

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

		Data maddr = sipUrl->getMaddrParam();
		if (maddr.length() > 0)
		{
		    data += ";";
		    data += "maddr=";
		    data += maddr;
		}
                if (sipUrl->isLooseRouterPresent())
                {
                    data += ";lr";
                }

		data += ">";
	    }
	}
    }
    return data;
}


const SipRoute& 
SipRoute::operator =(const SipRoute& src)
{
    if ( &src != this )
    {
	url = duplicateUrl(src.url);
	urlType = src.urlType;
        displayname = src.displayname;
    }

    return *this;
}


bool 
SipRoute::operator ==(const SipRoute& src) const
{
    bool equal = false;
    if ( (url.getPtr() != 0)  && (src.url.getPtr() != 0) )
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
    return   ( equal  &&
	     (  displayname == src.displayname )
	     );
}


SipHeader*
SipRoute::duplicate() const
{
    return new SipRoute(*this);
}


bool
SipRoute::compareSipHeader(SipHeader* msg) const
{
    SipRoute* otherMsg = dynamic_cast<SipRoute*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

ostream&
Vocal::operator<<(ostream& s, const SipRoute& route)
{
    s << route.encode();
    return s;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
