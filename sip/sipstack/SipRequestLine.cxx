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

static const char* const SipRequestLine_cxx_Version =
    "$Id: SipRequestLine.cxx,v 1.3 2004/11/19 01:54:38 greear Exp $";

#include "global.h"
#include "SipRequestLine.hxx"
#include "symbols.hxx"
#include "SystemInfo.hxx"
#include "cpLog.h"
#include "SipMethod.hxx"

using namespace Vocal;

#if 0
static const Data UnknownSipMethod(UNKNOWN_METHOD.getData());
static const Data DefaultSipVersion(DEFAULT_VERSION.getData());
static const Data DefaultSipProtocol(DEFAULT_PROTOCOL.getData());
#endif

string
SipRequestLineParserException::getName( void ) const
{
    return "SipRequestLineParserException";
}


SipRequestLine::SipRequestLine(Method amethod, Sptr <BaseUrl> url, const string& _local_ip)
    : local_ip(_local_ip)
{
    //initialize the default port, and over write if present in the url.

    // set the default for protocol and version
    protocol = Data(DEFAULT_PROTOCOL);
    version = Data(DEFAULT_VERSION);

    requestUrl = url;
    method = methodToData(amethod);
}


SipRequestLine::SipRequestLine(const Data& data, Sptr<BaseUrl> url, const string& _local_ip)
    : local_ip(_local_ip)
{
    method = data;
    requestUrl = url;
}
    
    
SipRequestLine::SipRequestLine(const string& _local_ip, UrlType type)
    : method(UNKNOWN_METHOD),
      version(DEFAULT_VERSION),
      protocol(DEFAULT_PROTOCOL),
      local_ip(_local_ip)    
{
    urlType = type;
}


SipRequestLine::SipRequestLine(const SipRequestLine& reqLine)
    : requestUrl(duplicateUrl(reqLine.requestUrl)),
      urlType(reqLine.urlType),
      method(reqLine.method),
      version(reqLine.version),
      protocol(reqLine.protocol),
      local_ip(reqLine.local_ip)
{
}


SipRequestLine::SipRequestLine( const Data& srcData, const string& _local_ip )
    : local_ip(_local_ip)
{
    Data fdata = srcData;
    try
    {
        decode(fdata);
	if (requestUrl.getPtr() != 0)
	{
	    urlType = requestUrl->getType();  //to keep the two in sync.
	}
    }
    catch (SipRequestLineParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of Requestline :( ");
            throw SipRequestLineParserException(
                "failed to decode the Requestline string :(",
                __FILE__,
                __LINE__, DECODE_REQUESTLINE_FAILED);
        }
    }
}


void
SipRequestLine::decode(const Data& data) throw (SipRequestLineParserException&)
{
    Data nData = data;

    try
    {
        scanSipRequestLine(nData);
    }
    catch (SipRequestLineParserException&)
    {
        cpLog(LOG_ERR, "Failed to Decode in Constructor of Requestline, data: %s",
              data.c_str());
        throw SipRequestLineParserException(
            "failed to decode the RequestLine string",
            __FILE__,
            __LINE__, DECODE_REQUESTLINE_FAILED);
    }
}


void
SipRequestLine::scanSipRequestLine(const Data &tmpdata) 
    throw (SipRequestLineParserException&)
{
    Data reqdata = tmpdata;
    Data reqvalue;
    int ret = reqdata.match(" ", &reqvalue, true);
    if (ret == FOUND)
    {
        setMethod(reqvalue);
        Data reqsdata = reqdata;
        Data reqsvalue;
        int retn = reqsdata.match(" ", &reqsvalue, true);
        if (retn == FOUND)
        {
            try
            {
                if (requestUrl.getPtr() == 0)
		{
		    requestUrl = BaseUrl::decode(reqsvalue, getLocalIp());
		}
		else
		{
		    requestUrl->decode(reqsvalue, getLocalIp());
		}
            }
            catch (SipUrlParserException e)
            {
                cpLog(LOG_ERR, "Failed to decode URL of Requestline, reqData: %s",
                      reqdata.c_str());
                throw SipRequestLineParserException(
                    "failed to decode the RequestLine URL",
                    __FILE__,
                    __LINE__, DECODE_REQUESTLINE_FAILED);
            }

            Data finaldata = reqsdata;
            Data finalvalue;
            parseVersion(finaldata);
        }
        else if (retn == NOT_FOUND)
        {
            cpLog(LOG_ERR, 
                  "Failed to Decode in scanSipRequestline() of Requestline, reqdata: %s",
                  reqdata.c_str());
            throw SipRequestLineParserException(
                "failed to decode the RequestLine string",
                __FILE__,
                __LINE__, DECODE_REQUESTLINE_FAILED);
        }
        else if (retn == FIRST)
        {
            cpLog(LOG_ERR, 
                  "Failed to Decode in scanSipRequestline() of Requestline: %s ",
                  reqdata.c_str());
            throw SipRequestLineParserException(
                "failed to decode the RequestLine string",
                __FILE__,
                __LINE__, DECODE_REQUESTLINE_FAILED);
        }
    }
    else if (ret == NOT_FOUND)
    {
        cpLog(LOG_ERR, "Failed to Decode in scanSipRequestline() of Requestline, reqdata: %s",
              reqdata.c_str());
        throw SipRequestLineParserException(
            "failed to decode the RequestLine string",
            __FILE__,
            __LINE__, DECODE_REQUESTLINE_FAILED);
    }
    else if (ret == FIRST)
    {
        cpLog(LOG_ERR, "Failed to Decode in scanSipRequestline() of Requestline, reqdata: %s", reqdata.c_str());
        throw SipRequestLineParserException(
            "failed to decode the RequestLine string",
            __FILE__,
            __LINE__, DECODE_REQUESTLINE_FAILED);
    }
}


const SipRequestLine& 
SipRequestLine::operator=(const SipRequestLine& src)
{
    if (&src != this)
    {
        requestUrl = duplicateUrl(src.requestUrl);
        method = src.method;
        version = src.version;
        protocol = src.protocol;
    }

    return *this;
}


bool  
SipRequestLine::operator==(const SipRequestLine& src) const 
{
    bool equal = false;
    if ( (requestUrl.getPtr() != 0) && (src.requestUrl.getPtr() != 0) )
    {
	equal = ( requestUrl->areEqual(src.requestUrl));
    }
    else if ( (requestUrl.getPtr() == 0) && (src.requestUrl.getPtr() == 0) )
    {
	equal = true;
    }
    else
    {
	equal = false;
    }
    return  (equal && 
             (  method == src.method ) && 
             (  version == src.version ) && 
             (  protocol == src.protocol ) 
        );
}


Sptr <BaseUrl> 
SipRequestLine::getUrl() const
{
    return duplicateUrl(requestUrl, true);
}


void 
SipRequestLine::setUrl(Sptr <BaseUrl> url)
{
    requestUrl = duplicateUrl(url);
}


Data 
SipRequestLine::getMethod() const
{
    return method;
}


void 
SipRequestLine::setMethod(const Data& newmethod)
{
     method = newmethod;
}

    
Data 
SipRequestLine::encode() const
{
    Data siprequest;

    if (requestUrl.getPtr() != 0)
    {
	siprequest = method;
	siprequest += SP;
	siprequest += requestUrl->encode();
        if (requestUrl->isLooseRouterPresent())
        {
            siprequest += ";lr";
        }
	siprequest += SP;
	siprequest += protocol;
	siprequest += "/";
	siprequest += version;
	siprequest += CRLF;
    }
    return siprequest;
}


void 
SipRequestLine::setProtocol(const Data& newprotocol)
{
    protocol = newprotocol;

}


void 
SipRequestLine::parseVersion(const Data& newver)
{
    Data finaldata = newver;
    Data finvalue;
    int test = finaldata.match("/", &finvalue, true);
    if (test == FIRST)
    {
        //EXp
    }

    if (test == FOUND)
    {
        setVersion(finaldata);
        setProtocol(finvalue);
    }

    if (test == NOT_FOUND)
    {
        cpLog(LOG_ERR, "Failed to Decode version in Request line");
        throw SipRequestLineParserException(
            "failed to decode the RequestLine string",
            __FILE__,
            __LINE__, DECODE_REQUESTLINE_FAILED);
    }


}


void
SipRequestLine::setVersion(const Data& newversion)
{
    version = newversion;
}


Data 
SipRequestLine::getProtocol() const
{
    return protocol;
}


Data 
SipRequestLine::getVersion() const
{

    return version;
}


void 
SipRequestLine::setTransportParam(const Data& transport)
{
    //Valid values for transport "TCP" or "UDP" 
    assert((transport == "TCP") || (transport == "UDP"));
    Sptr<BaseUrl> bUrl = getUrl();
    Sptr<SipUrl> sUrl((SipUrl*)(bUrl.getPtr()));
    if(sUrl != 0)
    {
        if(transport == "TCP")
        {
            sUrl->setTransportParam("tcp");
        }
        else
        {   
            sUrl->setTransportParam("");
        }
        setUrl(bUrl);
    }
}

Data 
SipRequestLine::getTransportParam() const
{
    Data retVal;
    Sptr<BaseUrl> bUrl = getUrl();
    Sptr<SipUrl> sUrl((SipUrl*)(bUrl.getPtr()));
    if(sUrl != 0)
    {
        retVal = sUrl->getTransportParam();
    }
    return retVal;
}

Data
SipRequestLine::getHost() const
{
    Sptr<SipUrl> sUrl((SipUrl*)(requestUrl.getPtr()));
    assert(sUrl != 0);
    return sUrl->getHost();
}


void 
SipRequestLine::setHost(const Data& host)
{
    Sptr<SipUrl> sUrl((SipUrl*)(requestUrl.getPtr()));
    if(sUrl != 0)
    {
        sUrl->setHost(host);
    }
}


void 
SipRequestLine::setPort(const Data& port)
{
    Sptr<SipUrl> sUrl((SipUrl*)(requestUrl.getPtr()));
    if(sUrl != 0)
    {
        sUrl->setPort(port);
    }
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
