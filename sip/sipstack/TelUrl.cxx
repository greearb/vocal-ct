
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


static const char* const TelUrl_cxx_Version =
    "$Id: TelUrl.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "global.h"
#include "TelUrl.hxx"
#include "symbols.hxx"
#include "cpLog.h"
#include <cassert>

using namespace Vocal;


    
string
TelUrlParserException::getName( void ) const
{
    return "TelUrlParserException";
}    


TelUrl::TelUrl(const TelUrl& src)
        :
        BaseUrl(src),
        schemeName(src.schemeName),
        user(src.user)
{
}


TelUrl::TelUrl( const Data& data ) throw(TelUrlParserException&)
        :
        BaseUrl(),
        schemeName(),
        user()
{
    try
    {
        decode(data);
    }
    catch (TelUrlParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            throw TelUrlParserException("failed in Decode", __FILE__, __LINE__, TEL_DECODE_FAILED);
        }
    }
}

    
TelUrl::TelUrl()
        :
        BaseUrl(),
        schemeName(),
        user()
{
}


TelUrl::~TelUrl()
{
}

Sptr<BaseUrl> TelUrl::duplicate() const
{
    return new TelUrl(*this);
}    
    
bool TelUrl::areEqual(Sptr<BaseUrl> baseUrl) const
{
    return areEqual(*baseUrl);
}
    
    
bool TelUrl::isLessThan(Sptr<BaseUrl> baseUrl) const
{
    if (baseUrl->getType() == TEL_URL)
    {
        Sptr<TelUrl> newUrl;
        newUrl.dynamicCast(baseUrl);
        return ( *(this) < *(newUrl) );
    }
    else
    {
	return false;
    }
}
    

// comparison operator is used by others
bool 
TelUrl::areEqual(const BaseUrl& baseUrl) const
{
    if(baseUrl.getType() == SIP_URL)
    {
        const TelUrl& newUrl = dynamic_cast<const TelUrl&> (baseUrl);
        return ( *(this) == newUrl );
    }
    else
    {
        return false;
    }
}



TelUrl& TelUrl::operator =(const TelUrl& srcUrl)
{
    if (this != &srcUrl)
    {
	schemeName = srcUrl.schemeName;
	user = srcUrl.user;
    }
    return (*this);
}

    
bool TelUrl::operator<(const TelUrl& srcUrl) const
{
    if (schemeName < srcUrl.schemeName) 
    {
	return true;
    }
    else if ( (user.getPtr() != 0) && (srcUrl.user.getPtr() != 0) )
    {
        return ( (*user) < (*(srcUrl.user)) );
      
    }
    else if ( (user.getPtr() == 0) && (srcUrl.user.getPtr() != 0) )
    {
        return true;
    }
    return false;

}

    
bool 
TelUrl::setUserValue(const Data& data, const Data& dummy)
{
    if (user.getPtr() == 0)
    {
	user = new SipTelSubscriberUser();
    }
    return user->decode(data);
}
    
    
bool TelUrl::operator>(const TelUrl& srcUrl) const
{
    return ( srcUrl < *this );
}
    
    
bool TelUrl::operator ==(const TelUrl& srcUrl) const
{
    bool equal = false;
    cpLog(LOG_DEBUG_STACK, "Url operator ==  function");
    if (user.getPtr() != 0)
    {
	cpLog(LOG_DEBUG_STACK, "this.user= %s", user->encodeWithParams().logData());
    }
    if (srcUrl.user.getPtr() != 0)
    {
	cpLog(LOG_DEBUG_STACK, "other.user= %s", srcUrl.user->encodeWithParams().logData());
    }
    
    cpLog(LOG_DEBUG_STACK, "this.schemeName = " , schemeName.logData());
    cpLog(LOG_DEBUG_STACK, "other.schemeName= " , srcUrl.schemeName.logData());

    equal = (isEqualNoCase(schemeName, srcUrl.schemeName) );
    
    if ( (user.getPtr() != 0) && (srcUrl.user.getPtr() != 0) )
    {
	equal = equal && ( (*user) == ( *(srcUrl.user) ) );
    }
    else if ( (user.getPtr() == 0) && (srcUrl.user.getPtr() == 0) )
    {
	equal = equal && true;
    }
    
    
    cpLog(LOG_DEBUG_STACK, "TelUrl comparison: , returning: %s", (equal == true ? "true" : "false" ));
    return equal;
}

    
void
TelUrl::decode( const Data& urlstr )
{
    if (fastDecode(urlstr) == false) {
            cpLog(LOG_ERR, "Failed in Decode (TEL_URL)");
            throw TelUrlParserException("Failed in Decode (TEL_URL)", 
					__FILE__, __LINE__, TEL_NOT_VALID_URL_DATA);
    }
    cpLog(LOG_DEBUG_STACK, "completed decoding in the TelUrl");
}    // TelUrl::decode

    
bool
TelUrl::fastDecode( Data myData )
{
    // new decoder, hopefully faster

    bool parseFailed;
    
    Data urlType = myData.parse(":", &parseFailed);

    if(parseFailed)
    {
	cpLog(LOG_DEBUG, "parse failed: could not find ':' separating URL type from body");
	return false;
    }

    if (!isEqualNoCase(urlType, "tel") )
    {
	cpLog(LOG_DEBUG, "parse failed: scheme not TEL");
	return false;
    }
    //set it.
    schemeName = urlType;
    
    // now, pass the remaining to SipTelSubscriber.
    if (myData.length())
    {
	user = new SipTelSubscriberUser(myData);
    }
    if (user.getPtr() != 0)
    {
	return true;
    }
    else
    {
	return false;
    }
}


//
Data
TelUrl::encode() const
{
    // TEL-URL
    Data telUrl;

    if (schemeName.length())
    {
	telUrl = schemeName;
    }
   
    telUrl+= COLON;

    // userinfo = user [ ":" password ]
    if (user.getPtr() != 0)
    {
	Data userValue = user->encodeWithParams();
	if (userValue.length() > 0)
	{
	    telUrl += userValue;
	}
    }
   
    return telUrl;
}    // TelUrl::encode


Data
TelUrl::getUserValue() const
{
    Data userValue;
    if (user.getPtr() != 0)
    {
	userValue = user->encode();
    }
    return userValue;
}
    
   
    
void
TelUrl::setUser(Sptr <BaseUser> newUser)
{
    Sptr <SipTelSubscriberUser> telUser;
    
    telUser.dynamicCast(newUser);
    user = telUser;
}
    

Sptr <BaseUser> 
TelUrl::getUser() const
{
    const Sptr<BaseUser> baseUser = user;
    return baseUser;
}


UrlType
TelUrl::getType() const
{
    return TEL_URL;
}


Data 
TelUrl::getUniqueKey() const
{
    return getUserValue();
}    
    
//write more accessory functions to access SipTelSubscriberUser functions.

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
