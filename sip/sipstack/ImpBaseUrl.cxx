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

static const char* const ImpBaseUrl_cxx_Version =
    "$Id: ImpBaseUrl.cxx,v 1.2 2004/05/04 07:31:14 greear Exp $";

#include "global.h"
#include <cassert>
#include "symbols.hxx"
//#include "HostMatch.hxx"
#include "cpLog.h"
#include "Data.hxx"
#include "ImpBaseUrl.hxx"

using namespace Vocal;

ImpBaseUrl::ImpBaseUrl(const ImpBaseUrl& src)
        :BaseUrl(),
        schemeName(src.schemeName),
        user(src.user),
        host(src.host)
{
}


ImpBaseUrl::ImpBaseUrl( const Data& data , const Data sName) throw(ImpBaseUrlParserException&)
        :BaseUrl(),
        schemeName(sName),
        user(),
        host()
{
    try
    {
        decode(data);
    }
    catch (ImpBaseUrlParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            throw ImpBaseUrlParserException("failed in Decode", __FILE__, __LINE__, IMP_DECODE_FAILED);
        }
    }
}

ImpBaseUrl::ImpBaseUrl()
        :BaseUrl(),
        schemeName(),
        user(),
        host()
{
}



ImpBaseUrl::~ImpBaseUrl()
{
}

bool ImpBaseUrl::areEqual(Sptr<BaseUrl> baseUrl) const
{
    return areEqual(*baseUrl);
}
    
    
bool ImpBaseUrl::isLessThan(Sptr<BaseUrl> baseUrl) const
{
    if (baseUrl->getType() == getType())
    {
        Sptr<ImpBaseUrl> newUrl((ImpBaseUrl*)(baseUrl.getPtr()));
        return ( *(this) < *(newUrl) );
    }
    else
    {
	return false;
    }
}

bool
ImpBaseUrl::areEqual(const BaseUrl& baseUrl) const
{
    if(baseUrl.getType() == getType())
    {
        const ImpBaseUrl& newUrl = dynamic_cast<const ImpBaseUrl&> (baseUrl);
        return ( *(this) == newUrl );
    }
    else
    {
        return false;
    }
}
 
    
    
ImpBaseUrl& ImpBaseUrl::operator =(const ImpBaseUrl& srcUrl)
{ 
    if (&srcUrl != this) 
    {
        schemeName = srcUrl.schemeName;
	user = srcUrl.user;
	host = srcUrl.host;
    }
    return (*this);
}

bool ImpBaseUrl::operator<(const ImpBaseUrl& srcUrl) const
{
    //we won't compare hosts in this.
    if (user < srcUrl.user)
    {
        return true;
    }
    else
    {
	return false;
    }

}

bool ImpBaseUrl::operator>(const ImpBaseUrl& srcUrl) const
{
    //compare reverse.
    return ( srcUrl < (*this) );
}
    
    
bool ImpBaseUrl::operator ==(const ImpBaseUrl& srcUrl) const
{
    bool equal = false;
    cpLog(LOG_DEBUG_STACK, "Url operator ==  function");
   
    cpLog(LOG_DEBUG_STACK, "this.schemeName= %s" , schemeName.logData());
    cpLog(LOG_DEBUG_STACK, "other.schemeName= %s", srcUrl.schemeName.logData());

    cpLog(LOG_DEBUG_STACK, "this.user= %s" , user.logData());
    cpLog(LOG_DEBUG_STACK, "other.user= %s", srcUrl.user.logData());
    
    cpLog(LOG_DEBUG_STACK, "this.host= %s" , host.logData());
    cpLog(LOG_DEBUG_STACK, "other.host= %s", srcUrl.host.logData());
    
    equal =  ((isEqualNoCase(schemeName, srcUrl.schemeName)) &&
	     (isEqualNoCase(user, srcUrl.user)) &&
             (isEqualNoCase(host, srcUrl.host) ) ); 

    
    cpLog(LOG_DEBUG_STACK, "ImpBaseUrl comparison: , returning: %s", (equal == true ? "true" : "false" ));
    return equal;
    
}
    

void
ImpBaseUrl::decode( const Data& urlstr )
{
    if (fastDecode(urlstr) == false) {
            cpLog(LOG_ERR, "Failed in Decode (%s) URL", schemeName.logData());
            throw ImpBaseUrlParserException("Failed in Decode URL", 
					__FILE__, __LINE__, IMP_NOT_VALID_URL_DATA);
    }
    cpLog(LOG_DEBUG_STACK, "completed decoding in the ImpBaseUrl");
}    // ImpBaseUrl::decode

bool
ImpBaseUrl::fastDecode( Data myData )
{
    bool parseFailed = true;
    
    Data urlType = myData.parse(":", &parseFailed);

    if(parseFailed)
    {
	cpLog(LOG_DEBUG, "parse failed: could not find ':' separating URL type from body");
	return false;
    }

    if  (!isEqualNoCase(urlType, schemeName) )
    {
	cpLog(LOG_DEBUG, "parse failed: scheme not %s", schemeName.logData());
	return false;
    }
    
    // now, either user or host, depending

    char matchedChar;

    Data tmpData;
    tmpData = myData.matchChar("@", &matchedChar);

    if(matchedChar == '@')
    {
	// this really is a user
	user = tmpData;
	// the next thing must be a host
	host = myData.matchChar(";", &matchedChar);
	if(matchedChar == ';')
	{
            cpLog(LOG_ERR, "Failed to parse (%s) URL", schemeName.logData());
            return false;
	}
	else
	{
	    //no separator, so the rest has to be a host
	    host = myData;
	    myData.erase();
	}
    }
    else
    {
        cpLog(LOG_ERR, "Failed to parse (%s) URL", schemeName.logData());
        return false;
    }

    return true;
}


///
Data
ImpBaseUrl::encode() const
{
    Data imurl;

    if (schemeName.length())
    {
	imurl = schemeName;
    }
    imurl+= COLON;

    // userinfo = user 
    if (user.length() != 0)
    {
        imurl += user;
        imurl += "@";
    }

    imurl += host;

    return imurl;
}    // ImpBaseUrl::encode


Data
ImpBaseUrl::getNameAddr() const
{
    Data nameaddr;
    if (schemeName.length())
    {
	nameaddr = schemeName;
	nameaddr += COLON;
    }
    
    if (user.length())
    {
	nameaddr += user;
        nameaddr += "@";
    }
    nameaddr += host;
    return nameaddr;
}


Data
ImpBaseUrl::getUniqueKey() const
{
    return getNameAddr();
}




Data
ImpBaseUrl::getUserValue() const
{
    return user;
}

bool 
ImpBaseUrl::setUserValue(const Data& newUser, const Data& noUse)
{
    user = newUser;
    return true;
    
}

    
const Data&
ImpBaseUrl:: getHost() const
{
    return host;
}

    
void
ImpBaseUrl::setHost(const Data& newhost)
{
    host = newhost;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
