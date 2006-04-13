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
#include "SipFrom.hxx"
#include "symbols.hxx"
#include "cpLog.h"
#include "SipParserMode.hxx"
#include "SipUrl.hxx"
#include "TelUrl.hxx"
#include "NullUrl.hxx"
#include "SipTo.hxx"

using namespace Vocal;


string
SipFromParserException::getName( void ) const
{
    return "SipFromParserException";
}


SipFrom::SipFrom(Sptr <BaseUrl> src, const string& local_ip)
    : SipHeader(local_ip),
        displayName(),
        fromUrl(duplicateUrl(src)),
        urlType(UNKNOWN_URL),
        tag(),
        token(),
        qstring(),
        tokenMap()

{
    if (fromUrl != 0)
    {
	if (fromUrl->getType() == SIP_URL)
	{
            urlType = SIP_URL;
	    Sptr <SipUrl> sipUrl((SipUrl*)(fromUrl.getPtr()));
	    sipUrl->initializeFrom();
	}
    }
}


SipFrom::SipFrom(const SipFrom& src)
    : SipHeader(src),
        displayName(src.displayName),
        fromUrl(duplicateUrl(src.fromUrl)),
        urlType(src.urlType),
        tag(src.tag),
        token(src.token),
        qstring(src.qstring),
        tokenMap(src.tokenMap)

{
    if (fromUrl != 0)
    {
	if (fromUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(fromUrl.getPtr()));
	    sipUrl->initializeFrom();
	}
    }
}

SipFrom::SipFrom(const SipTo& src) 
    : SipHeader(src.getLocalIp()),
      displayName(src.getDisplayName()),
      fromUrl(src.getUrl()),
      urlType(fromUrl->getType()),
      tag(src.getTag()),
      token(src.getToken()),
      qstring(src.getQstring()),
      tokenMap(src.getTokenDetails())
{
    if (fromUrl != 0)
    {
	if (fromUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(fromUrl.getPtr()));
	    sipUrl->initializeFrom();
	}
    }
}

const SipFrom& 
SipFrom::operator =(const SipFrom& src)
{
    if (&src != this)
    {
        displayName = src.displayName;
        fromUrl = duplicateUrl(src.fromUrl);
        urlType = src.urlType;
        tag = src.tag;
        token = src.token;
        qstring = src.qstring;
        tokenMap = src.tokenMap;
	if (fromUrl != 0)
	{
	    if (fromUrl->getType() == SIP_URL)
	    {
		Sptr <SipUrl> sipUrl((SipUrl*)(fromUrl.getPtr()));
		sipUrl->initializeFrom();
	    }
	}

    }
    return (*this);
}

SipFrom::SipFrom( const Data& data, const string& local_ip, UrlType uType )
    : SipHeader(local_ip),
        displayName(),
        fromUrl(),
        tag(),
        qstring(),
        tokenMap()
{
    if (data == "") {
        urlType = uType;  //will be used to create Url objects, during set operation
        return;
    }

    try
    {
        decode(data);
	if (fromUrl != 0)
	{
	    if (fromUrl->getType() == SIP_URL)
	    {
		Sptr <SipUrl> sipUrl((SipUrl*)(fromUrl.getPtr()));
		sipUrl->initializeFrom();
	    }
	}
    }
    catch (SipFromParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipFromParserException(
                "failed to decode the From string",
                __FILE__,
                __LINE__, DECODE_FROM_FAILED);
        }
    }
}


void
SipFrom::scanSipFrom( const Data & tmpdata)
{
    Data sipdata;
    Data data = tmpdata;
    int ret = data.match("<", &sipdata, true);
    if (ret == NOT_FOUND)
    {
        Data value;
        Data tmpval;
        int retn = data.match(";", &value, true) ;
        if (retn == NOT_FOUND)
        {  // it's Ok becos it Can be URl only since Addrs. Params are Optional
            tmpval = value;

            fromUrl = BaseUrl::decode(data, getLocalIp());
            urlType = fromUrl->getType();
        }
        else if (retn == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in ScanSIPFrom :o( ");
                throw SipFromParserException("failed in Decode", __FILE__, __LINE__, DECODE_FROM_FAILED);

            }

        }
        else if (retn == FOUND)
        { // if found then it has the Addrs-params
            tmpval = value;

            fromUrl = BaseUrl::decode(tmpval, getLocalIp());
            value = data;
            parseAddrParam(value);
        }

    }
    else if (ret == FIRST)
    {
        try
        {
            parseUrl(data);
        }
        catch (SipFromParserException&)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in ScanSIPFrom :o( ");
                throw SipFromParserException("failed in Decode in scanSipFrom", __FILE__, __LINE__, URL_FAILED);

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
SipFrom::parseUrl(const Data& data)
{
    Data urlvalue = data;
    Data avalue;
    int retur = urlvalue.match(">", &avalue, true);
    if (retur == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl() :o( ");
            throw SipFromParserException("failed in Decode", __FILE__, __LINE__, URL_FAILED);

        }
    }
    else if (retur == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl()  :o( ");
            throw SipFromParserException("failed in Decode", __FILE__, __LINE__, URL_FAILED);

        }
    }
    else if (retur == FOUND)
    {
        avalue.removeSpaces(); //Fix for PINGTEL SPACES
   
        fromUrl = BaseUrl::decode(avalue, getLocalIp());
        Data te = urlvalue;
        Data fik;
        //remove spaces since of form <url>SP;tag= (pingtel bug)
        te.removeSpaces();
        int rety = te.match(";", &fik, true);
        if (rety == FOUND)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Parse in ParseUrl()  :o( ");
                throw SipFromParserException("failed in Decode", __FILE__, __LINE__, URL_FAILED);

            }
        }
        else if (rety == FIRST)
        {
            parseAddrParam(te);
        }
        else if (rety == NOT_FOUND)
        {}

    }
}


void
SipFrom::parseNameInfo(const Data& data)
{
    Data newnameinfo;
    Data nameinfo = data;
    //nameinfo.removeSpaces();  // !jf! to handle problem with 7960
    setDisplayName(nameinfo);

}


void
SipFrom::parseAddrParam(const Data& data)
{
    Data addrparm = data;
    Data parm;
    while (1)
    {
        int ret = addrparm.match(";", &parm, true);
        if (ret == NOT_FOUND)
        {
            parseAddrParamExt(addrparm);
            break;
        }
        else if (ret == FIRST)
        {}

        else if (ret == FOUND)
        {
            parseAddrParamExt(parm);
        }
    }
}


void
SipFrom::parseAddrParamExt(const Data& newdat)
{
    Data checkdata = newdat;
    Data parmvalue;
    int retn = checkdata.match("=", &parmvalue, true);
    if (retn == NOT_FOUND)
    {
        tokenMap[checkdata] = "";
    }
    else if (retn == FIRST)
    {
    }

    else if (retn == FOUND)
    {
        parmvalue.removeSpaces();
        parseTag(parmvalue, checkdata);
    }
}


void
SipFrom::parseTag(const Data& ltoken, const Data& vdata)
{
    if (ltoken == TAG_CHECK)
    {
        tag = vdata;
        tag.removeSpaces();
    }
    else
    {
        Data copy = ltoken;
        Data copy2 = vdata;
        copy.removeSpaces();
        copy2.removeSpaces();

        tokenMap[copy] = copy2;
    }
}


void
SipFrom::parse( const Data& fromdata)
{

    Data data = fromdata;

    try
    {

        scanSipFrom(data);
    }
    catch (SipFromParserException& exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");

            throw SipFromParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, DECODE_FROM_FAILED
            );
        }
    }


    //everything allright.


}


void
SipFrom::decode( const Data& fromstr )
{

    try
    {
        parse(fromstr);
    }
    catch (SipFromParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");

            throw SipFromParserException(
                "Parser Exception",
                __FILE__,
                __LINE__, DECODE_FROM_FAILED);
        }
    }
}


const bool 
SipFrom::operator<(const SipFrom& srcFrom) const
{
    bool urlLess = false;
    if ( (fromUrl != 0) && (srcFrom.fromUrl != 0) )
    {
	urlLess = ( fromUrl->isLessThan(srcFrom.fromUrl));
    }
    else if ( (fromUrl == 0) && (srcFrom.fromUrl != 0) )
    {
	urlLess = true;
    }
    if (urlLess  ||
        (tag < srcFrom.tag) ||
        (token < srcFrom.token) ||
        (qstring < srcFrom.qstring)
    )
    {
        return true;
    }
    return false;
}


bool 
SipFrom::operator>(const SipFrom& srcFrom) const
{
    return (srcFrom < *this);
}


bool 
SipFrom::operator ==(const SipFrom& srcFrom) const
{
    bool equal = false;

    if ( (fromUrl != 0) && (srcFrom.fromUrl != 0) )
    {
        equal = (fromUrl->areEqual(srcFrom.fromUrl));
    }
    else if ( (fromUrl == 0) && (srcFrom.fromUrl == 0) )
    {
	equal = true;
    }
    else
    {
        equal = false;
    }

    cpLog(LOG_DEBUG_STACK, " from urls match result: %s", 
          (equal ? "==" : "!=") );
    equal = (equal && (tag == srcFrom.tag));

    cpLog(LOG_DEBUG_STACK, " tags match result: %s", (equal ? "==" : "!=") );

    equal = (equal && (tokenMap == srcFrom.tokenMap));
    cpLog(LOG_DEBUG_STACK, "tokenMap match result: %s", 
          (equal ? "==" : "!=") );

    cpLog(LOG_DEBUG_STACK, "final result of From operator == is: %s ",
          ( equal ? "true" : "false" ) );
    return equal;
}


int
SipFrom::getPort() const
{
    int port=0;
    
    if (fromUrl != 0)
    {
	if (fromUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl((SipUrl*)(fromUrl.getPtr()));
	    port = sipUrl->getPort().convertInt();
	}

    }
    return port;
}

    
void SipFrom::setPort(const Data& newport) {
   if (urlType == TEL_URL) {
      return;
   }
   if (fromUrl == 0) {
      fromUrl = new SipUrl("", getLocalIp(), false);
   }
   if (fromUrl != 0) {
      if (fromUrl->getType() == SIP_URL) {
         Sptr<SipUrl> sipUrl;
         sipUrl.dynamicCast(fromUrl);
         sipUrl->setPort(newport);
      }
      
   }
}

void SipFrom::setPort(int newport) {
   if (urlType == TEL_URL) {
      return;
   }
   if (fromUrl == 0) {
      fromUrl = new SipUrl("", getLocalIp(), false);
   }
   if (fromUrl != 0) {
      if (fromUrl->getType() == SIP_URL) {
         Sptr<SipUrl> sipUrl;
         sipUrl.dynamicCast(fromUrl);
         sipUrl->setPort(newport);
      }
      
   }
}


Data SipFrom::encode() const
{
    //assign to data, and return.
    Data sipFrom;
    
    if (fromUrl != 0)
    {
	sipFrom = FROM;
	sipFrom += SP;
	Data dispName = getDisplayName();
        dispName.removeSpaces();
        if (dispName != "") {
           if (dispName[0] != '\"') {
              sipFrom += "\"";
           }
           sipFrom += dispName;
           if (dispName[dispName.length() - 1] != '\"') {
              sipFrom += "\"";
           }
           sipFrom += SP;
        }
	sipFrom += "<";
	sipFrom += fromUrl->encode();
	sipFrom+= ">";
    }
    
    Data tagparam = getTag();
    if (tagparam.length() > 0)
    {
        sipFrom += ";";
        sipFrom += "tag=";
        sipFrom += tagparam;
    }
    TokenMapFrom::const_iterator i = tokenMap.begin();
    if (i != tokenMap.end())
    {
        sipFrom += ";";
    }
    while (i != tokenMap.end())
    {
        Data token1 = i->first;
        Data tokenValue1 = i->second;
        sipFrom += token1;
        if (tokenValue1.length() > 0)
        {
            sipFrom += "=";
            sipFrom += tokenValue1;
        }

        ++i;

        if ( i != tokenMap.end())
        {
            sipFrom += ";";
        }
    }  //end while
    //other than basic

    sipFrom += CRLF;
    
    return sipFrom;
}


void 
SipFrom::setTokenDetails(const Data& intoken, const Data& tokenValue)
{
    tokenMap[intoken] = tokenValue;
}


const SipFrom::TokenMapFrom&
SipFrom::getTokenDetails() const
{
    return tokenMap;
}


Data 
SipFrom::getTokenValue(const Data& intoken) const
{
    Data ret;
    TokenMapFrom::const_iterator check = tokenMap.find(intoken);
    if (check != tokenMap.end())
    {
        ret = check -> second;
    }

    return ret;

}


void 
SipFrom::setUser(const Data& newuser)
{
    if (fromUrl == 0)
    {
	if (urlType == SIP_URL)
	{
	    fromUrl = new SipUrl("", getLocalIp());
	}
	else if (urlType == TEL_URL)
	{
	    fromUrl = new TelUrl();
	}
    }
    //HERE, the user could be user for TelUrl. We need to be able to
    //identify it some how, Maybe, when we form the From empty object, pass this in the constructor.
    
    
    if (fromUrl != 0)
    {
	fromUrl->setUserValue(newuser);
    }
    //display name defaulted to user.
    if (displayName.length() == 0)
    {
        displayName = newuser;
    }
}


Data 
SipFrom::getUser() const
{
    Data user;
    if (fromUrl != 0)
    {
	user = fromUrl->getUserValue();
    }
    return user;
}

    
void 
SipFrom::setDisplayName(const Data& name)
{
    displayName = name;
}

    
const Data& 
SipFrom::getDisplayName() const
{
    return displayName;
}

    
void
SipFrom::setHost(const Data& newhost)
{
    if (urlType == TEL_URL)
    {
	return;
    }
    if (fromUrl == 0)
    {
	fromUrl = new SipUrl("", getLocalIp());
    }
    if (fromUrl != 0)
    {
    	if (fromUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl((SipUrl*)(fromUrl.getPtr()));
	    sipUrl->setHost(newhost);
	}
    }
}


Data
SipFrom::getHost() const
{
    Data host;
    if (fromUrl != 0)
    {
    	if (fromUrl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl((SipUrl*)(fromUrl.getPtr()));
	    host = sipUrl->getHost();
	}
    }
    return host;
}


const Data&
SipFrom::getTag() const
{
    return tag;
}


void
SipFrom::setTag( const Data& newtag )
{
    tag = newtag;
}


Sptr <BaseUrl>
SipFrom::getUrl() const
{
    return duplicateUrl(fromUrl, true);
}


void
SipFrom::setUrl( Sptr <BaseUrl> newfromUrl )
{
    fromUrl = duplicateUrl(newfromUrl);
}


Sptr<SipHeader>
SipFrom::duplicate() const
{
    return new SipFrom(*this);
}


bool
SipFrom::compareSipHeader(SipHeader* msg) const
{
    SipFrom* otherMsg = dynamic_cast<SipFrom*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}


