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
#include "SipUrl.hxx"
#include "symbols.hxx"
#include "cpLog.h"
#include "Data.hxx"
#include <cassert>
#include "SipUser.hxx"
#include "SipTelSubscriberUser.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;

//const Data DefaultSipPort(DEFAULT_SIP_PORT);

string
SipUrlParserException::getName( void ) const
{
    return "SipUrlParserException";
}

void SipUrl::initializeFrom()
{
    transportParam.erase();
    maddrParam.erase();
    headers.erase();
    lrParam = false;
}
    

void SipUrl::initializeTo()
{
    transportParam.erase();
    maddrParam.erase();
    ttlParam.erase();
    lrParam = false;
}


SipUrl::SipUrl(const SipUrl& src)
    :BaseUrl(),
     local_ip(src.local_ip),
     schemeName(src.schemeName),
     passwd(src.passwd),
     host(src.host),
     port(src.port),
     transportParam(src.transportParam),
     ttlParam(src.ttlParam),
     maddrParam(src.maddrParam),
     userParam(src.userParam),
     methodParam(src.methodParam),
     otherParam(src.otherParam),
     otherName(src.otherName),
     headers(src.headers),
     lrParam(src.lrParam),
     ipv6Addr(src.ipv6Addr),
     EscObj(src.EscObj)
{
    if (src.user != 0)
    {
        user = src.user->duplicate(); //Sptr of BaseUser now.
    }
}


SipUrl::SipUrl( const Data& data, const string& _local_ip, bool lr )
    :BaseUrl(),
     local_ip(_local_ip),
     schemeName("sip"),
     user(),
     passwd(),
     host(),
     port(),
     transportParam(),
     ttlParam(),
     maddrParam(),
     userParam(),
     methodParam(),
     otherParam(),
     otherName(),
     headers(),
     lrParam(lr),
     ipv6Addr(false),
     EscObj()
{

    // Make sure we were really passed an IP as first arg
    // TODO:  Remove this some day.
//    unsigned long foo;
//    assert(SystemInfo::toIpv4String(local_ip.c_str(), foo));
//    assert(strstr(local_ip.c_str(), ":") == NULL);

    if (data == "") {
        return;
    }

    try
    {
        decode(data);
    }
    catch (SipUrlParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            throw SipUrlParserException("failed in Decode", __FILE__, __LINE__, DECODE_FAILED);
        }
    }
}


UrlType
SipUrl::getType() const
{
    return SIP_URL;
}

    
Sptr<BaseUrl> SipUrl::duplicate() const
{
    return new SipUrl(*this);
}
    
    
bool SipUrl::areEqual(Sptr<BaseUrl> baseUrl) const
{
    if (baseUrl != 0)
    {        
        return areEqual(*baseUrl);
    }
    else
    {
        return false;
    }
}
    
    
bool SipUrl::isLessThan(Sptr<BaseUrl> baseUrl) const
{    
    if (baseUrl != 0 && baseUrl->getType() == SIP_URL)
    {
        Sptr<SipUrl> newUrl((SipUrl*)(baseUrl.getPtr()));
        return ( *(this) < *(newUrl) );
    }
    else
    {
	return false;
    }
}

bool
SipUrl::areEqual(const BaseUrl& baseUrl) const
{
    if(baseUrl.getType() == SIP_URL)
    {
        const SipUrl& newUrl = dynamic_cast<const SipUrl&> (baseUrl);
        return ( *(this) == newUrl );
    }
    else
    {
        return false;
    }
}
    
    
SipUrl& SipUrl::operator =(const SipUrl& srcUrl)
{ 
    if (&srcUrl != this) 
    {
	schemeName = srcUrl.schemeName;
        if(srcUrl.user != 0)
	   user = srcUrl.user->duplicate();
	passwd = srcUrl.passwd;
	host = srcUrl.host;
	port = srcUrl.port;
	transportParam = srcUrl.transportParam;
	ttlParam = srcUrl.ttlParam;
	maddrParam = srcUrl.maddrParam;
	userParam = srcUrl.userParam;
	methodParam = srcUrl.methodParam;
	otherParam = srcUrl.otherParam;
	headers = srcUrl.headers;
        lrParam = srcUrl.lrParam;
        ipv6Addr = srcUrl.ipv6Addr;
        if(srcUrl.EscObj != 0)
            EscObj = srcUrl.EscObj;
        local_ip = srcUrl.local_ip;
    }
    return (*this);
}

#define IsLess(a, b) if ((a) < (b)) return true; else if ((b) < (a)) return false

bool SipUrl::operator<(const SipUrl& src) const
{
    IsLess(user, src.user); //this will call SipUser, or SipTelSubscriberUsr isLessThan
    IsLess(passwd, src.passwd);
    IsLess(ttlParam, src.ttlParam);
    IsLess(maddrParam, src.maddrParam);
    IsLess(userParam, src.userParam);
    IsLess(methodParam, src.methodParam);
    IsLess(otherParam, src.otherParam);
    IsLess(port, src.port);    
    IsLess(headers, src.headers);
    
    return false;
}
#undef IsLess

bool SipUrl::operator>(const SipUrl& srcUrl) const
{
    //compare reverse.
    return ( srcUrl < (*this) );
}
    
    
bool SipUrl::operator ==(const SipUrl& srcUrl) const
{
    bool equal = false;
    cpLog(LOG_DEBUG_STACK, "Url operator ==  function");
    if (user != 0)
    {
	cpLog(LOG_DEBUG_STACK, "this.user= %s", user->encode().logData());
    }
    if (srcUrl.user != 0)
    {
	cpLog(LOG_DEBUG_STACK, "other.user= %s", srcUrl.user->encode().logData());
    }
    cpLog(LOG_DEBUG_STACK, "this.passwd= %s" , passwd.logData());
    cpLog(LOG_DEBUG_STACK, "other.passwd= %s", srcUrl.passwd.logData());
    
    cpLog(LOG_DEBUG_STACK, "this.host= %s" , host.logData());
    cpLog(LOG_DEBUG_STACK, "other.host= %s", srcUrl.host.logData());
    
    cpLog(LOG_DEBUG_STACK, "this.ttlParam= %s", ttlParam.logData() );
    cpLog(LOG_DEBUG_STACK, "other.ttlParam= %s" , srcUrl.ttlParam.logData());
    
    cpLog(LOG_DEBUG_STACK, "this.maddrParam= %s" , maddrParam.logData());
    cpLog(LOG_DEBUG_STACK, "other.maddrParam= %s" , 
	                                    srcUrl.maddrParam.logData());
    
    cpLog(LOG_DEBUG_STACK, "this.userParam= %s " , userParam.logData());
    cpLog(LOG_DEBUG_STACK, "other.userParam= %s" , srcUrl.userParam.logData());

    cpLog(LOG_DEBUG_STACK, "this.methodParam= %s" , methodParam.logData());
    cpLog(LOG_DEBUG_STACK, "other.methodParam= %s" , srcUrl.methodParam.logData());
    cpLog(LOG_DEBUG_STACK, "this.otherParam= %s", otherParam.logData());
    cpLog(LOG_DEBUG_STACK, "other.otherParam= %s" , 
	                      srcUrl.otherParam.logData());
    
    cpLog(LOG_DEBUG_STACK, "this.headers = %s" , headers.logData());
    cpLog(LOG_DEBUG_STACK, "other.headers = %s" , srcUrl.headers.logData());

    
    //compare user if it is defined.
    if ( (user != 0) && (srcUrl.user != 0) )
    {
        equal = ( user->areEqual(srcUrl.user) );
    }
    else if ( (user.getPtr() == 0) && (srcUrl.user.getPtr() == 0) )
    {
	//both are empty.
	equal = true;
    }
    else
    {
	equal = false;
    }
    
    //take care of some header comparsions requiring case sensitivity, and
    //some requiring case-insensitivity.
    equal =  (equal && 
	     (isEqualNoCase(schemeName, srcUrl.schemeName)) &&
	     (passwd == srcUrl.passwd) &&
             (isEqualNoCase(host, srcUrl.host) ) &&
             (isEqualNoCase(ttlParam,srcUrl.ttlParam)) &&
             (maddrParam == srcUrl.maddrParam) &&
             (isEqualNoCase(userParam,srcUrl.userParam) ) &&
             (methodParam == srcUrl.methodParam) &&
             (isEqualNoCase(otherParam, srcUrl.otherParam)) &&
             (isEqualNoCase(headers, srcUrl.headers)) );

    //compare the parameters having default values.
    if ( srcUrl.port.length() && port.length() )
    {
        equal = (equal && (port == srcUrl.port));
    }
    else
    {
	// one or the other is a default

	// the two ? : expressions evaluate to either the default
	// value (in this case Data("5060") if there is nothing in the
	// port) or the port (which is a Data) if there is something
	// there.  Thus, when they are compared via == , you do a
	// comparison which substitutes the default value if they are
	// not supplied.

	equal = (equal && 
		 (( (srcUrl.port.length() == 0) ? Data("5060") : srcUrl.port) 
		  ==
		  ( (port.length() == 0) ? Data("5060") : port)));
    }
    if (( srcUrl.transportParam.length() && transportParam.length()) )
    {
	equal = (equal && (transportParam == srcUrl.transportParam));
    }
    else
    {

	// the two ? : expressions evaluate to either the default
	// value (in this case Data("udp") if there is nothing in the
	// port) or the port (which is a Data) if there is something
	// there.  Thus, when they are compared via == , you do a
	// comparison which substitutes the default value if they are
	// not supplied.

	equal = (equal && 
		 (( (srcUrl.transportParam.length() == 0) ? 
		    Data("udp") : srcUrl.transportParam)  ==
		  ( (transportParam.length() == 0) ? Data("udp") : transportParam)));
    }
    
    
    cpLog(LOG_DEBUG_STACK, "sipUrl comparison: , returning: %s", (equal == true ? "true" : "false" ));
    return equal;
    
}
    

void
SipUrl::decode( const Data& urlstr )
{
    if (fastDecode(urlstr) == false) 
    {
        cpLog(LOG_ERR, "Failed in Decode (SIP_URL): %s", urlstr.logData());
        throw SipUrlParserException("Failed in Decode (SIP_URL)", 
                                    __FILE__, __LINE__, NOT_VALID_URL_DATA);
    }
    //cpLog(LOG_DEBUG_STACK, "completed decoding in the SipUrl");
}    // SipUrl::decode

bool
SipUrl::fastDecode( Data myData )
{
    // new decoder, hopefully faster
    bool userValid  = false;
    bool hostValid = false;
    bool portValid = false;
    bool userParamValid = false; // used to figure if the user is 
    //telUser or sipUser to parse differently.
    bool parseFailed;    
    string::size_type keyPos;
    string tmpmyData;
    string workstring = myData.convertString();
    if((keyPos = workstring.find_first_of("?")) != string::npos){
        string escData;
        tmpmyData = workstring.substr(0, keyPos);
        escData = workstring.substr(keyPos+1, string::npos);
        EscObj = new EmbeddedObj(escData, local_ip);
        myData = tmpmyData;   
    }
    Data userInfo;

    Data urlType = myData.parse(":", &parseFailed);

    if(parseFailed)
    {
	cpLog(LOG_DEBUG, "parse failed: could not find ':' separating URL type from body: %s", myData.logData());
	return false;
    }

    if  (!isEqualNoCase(urlType, "sip") )
    {
	cpLog(LOG_DEBUG, "parse failed: scheme not SIP: %s", myData.logData());
	return false;
    }
    //set it.
    schemeName = urlType;
    
    // now, either user or host, depending

    char matchedChar;

    Data tmpData;
    bool ret;
    tmpData = myData.matchChar("@", &matchedChar);
    if(matchedChar == '@')
    {
	// this really is a user
	userInfo = tmpData;
	userValid = true;
	// the next thing must be a host, optionally followed by a port
        //Check to see if the address is a IPV6 address
        host = myData.matchChar("[", &matchedChar);
        if(matchedChar == '[')
        {
            //Ok this is a IPv6 address
            ret = parseIpv6Address(myData);
            if(!ret) return ret;
        }
        if(host.length())
        {
           //It was a IPV6 address
           //eat away extra :;
           Data eData = myData.matchChar(":;", &matchedChar); 
	   hostValid = true;
        }
        else
        {
            //Take the address to be IPV4 address
	    host = myData.matchChar(":;", &matchedChar);
            if(host.length())
	        hostValid = true;
            
        }
	if (matchedChar == ':')
	{
	    // the next bit is a port
	    port = myData.matchChar(";", &matchedChar);
	    portValid = true;
	    if(matchedChar == ';')
	    {
		// the rest is fine
	    }
	    else
	    {
		// no match -- the rest of myData must be a port
		port = myData;
		myData.erase();
	    }
	}
	else if(matchedChar == ';')
	{
	}
	else
	{
	    // maybe there is no separator, so the rest has to be a host
	    host = myData;
	    myData.erase();
	    hostValid = true;
	}
    }
    else
    {
        //No user part, so parse the host
        tmpData = myData.matchChar("[", &matchedChar);
        if(matchedChar == '[')
        {
            //Ok this is a IPv6 address
            ret = parseIpv6Address(myData);
            if(!ret) return ret;
	    hostValid = true;
        }
        if(host.length())
        {
            //Since it was already IPV6 address
            //eat away extra :; 
            Data eData = myData.matchChar(":;", &matchedChar);
        }
        else
        {
            host = myData.matchChar(":;", &matchedChar);
            if(host.length())
	         hostValid = true;
        }
        if (matchedChar == ':')
        {
        	// this is not a user -- this is a host
	    port = myData.matchChar(";", &matchedChar);
	    if(matchedChar == ';')
	    {
	        // this is a port, if there is anything
	        portValid = true;
	    }
	    else
	    {
	        // myData must contain the port
	        port = myData;
	        myData.erase();
	        portValid = true;
	    }
        }
        else if (matchedChar == ';')
        {
           //No port do nothing
        }
        else if(!hostValid)
        {
           host = myData;
           hostValid = true;
	   myData.erase();
        }
    }

    if (!hostValid)
    {
        return false;
    }

    if (portValid)
    {
        for (int i=0; i<port.length(); i++)
        {
            if (!isdigit(port[i]))
            {
                return false;
            }
        }
    }

    // now, look for options, if there are any

    bool done = false;
    bool matchedParam = false;
    while(!done)
    {
	// look for an equal sign
	Data key = myData.matchChar("=", &matchedChar);
	Data value;

	if(matchedChar == '=')
	{
	    matchedParam = true;
    
	    // find the value
	    value = myData.matchChar(";?", &matchedChar);
	    if((matchedChar != ';') && (matchedChar != '?'))
	    {
		// did not match, so this must be the last parameter
		value = myData;
		myData.erase();
	    }
            else if(matchedChar == '?')
            {
                //Rest of it is Headers
                headers = myData;
                myData.erase();
	        matchedParam = false;
            }

	    // do something with the key-value pair
            key.removeSpaces();
	    if(key == "user")
	    {
		// do something here
		userParamValid = true;
		userParam = value;
	    }
	    else if(key == "transport")
	    {
		// do something here
		transportParam = value;
	    }
	    else if(key == "ttl")
	    {
		ttlParam = value;
	    }
	    else if(key == "maddr")
	    {
		maddrParam = value;
	    }
	    else if(key == "method")
	    {
		methodParam = value;
	    }
	    else if  (matchedParam)
	    {
		if (userParam == "phone")
		{
                    //we have a matchedparam, and value, but it is not
                    //a recognized one. This may be other Params or
                    //tel params.
		    userInfo+=";";
		    userInfo+=key;
		    userInfo+="=";
		    userInfo+=value;
		}
		else
		{
		    //belongs to SipUrl param.
		    otherName = key;
		    otherParam = value;
		}
	    }
	    matchedParam = false;  //reset it back.
	}
	else
	{
	    // done
	    done = true;
	}
    }
    
    if (myData == "lr")
    {
        lrParam = true;
    }

    if ( (!userParamValid) || ( (userParamValid) && (userParam == "ip") ) )
    {
	//did not find userParam. , or found ip in userParam.
        if (userValid)
	{
    
	    user  = new SipUser(userInfo);
	}
    }
    else //this is phone userparam.
    {
	if (userValid)
	{
	    user = new SipTelSubscriberUser(userInfo);
	}
    }

    //assert(strstr(host.c_str(), ":") == NULL);

    return true;
}


///
Data
SipUrl::encode() const
{
    // SIP-URL = "sip:" [ userinfo "@" ] hostport url-parameters [ headers ]
    Data sipurl;

    if (schemeName.length())
    {
	sipurl = schemeName;
    }
    sipurl+= COLON;

    // userinfo = user [ ":" password ]
    if (user.getPtr() != 0)
    {
        Data u = user->encode();
        u.removeSpaces();
        if (u.length() > 0) 
        {
            sipurl += user->encode();
            if (passwd.length() > 0)
            {
                sipurl += COLON;
                sipurl += passwd;
            }
            
            sipurl += "@";
        }
    }

    // hostport = host [ ":" port ]
    if(ipv6Addr)
    {
        sipurl += "[";
        sipurl += host;
        sipurl += "]";
    }
    else
    {
        sipurl += host;
    }
    if (port.length() > 0  && port != "0")
    {
        sipurl += COLON;
        sipurl += port;
    }

    // url-parameters = *( ";" url-parameter )
    // url-parameter = transport-param | user-param | method-param
    //               | ttl-param | maddr-param | other-param
    //               | tag-param

    // transport-param = "transport=" ( "udp" | "tcp" )
    if (transportParam.length() > 0)
    {
        sipurl += SEMICOLON;
        sipurl += SipUrlParamTransport;
        sipurl += transportParam;
    }

    // ttl-param = "ttl=" ttl
    if (ttlParam.length() > 0)
    {
        sipurl += SEMICOLON;
        sipurl += SipUrlParamTtl;
        sipurl += ttlParam;
    }

    // maddr-param = "maddr=" host
    if (maddrParam.length() > 0)
    {
        sipurl += SEMICOLON;
        sipurl += SipUrlParamMaddr;
        sipurl += maddrParam;
    }

    // user-param = "user=" ( "phone" | "ip" )
    if (userParam.length() > 0)
    {
        sipurl += SEMICOLON;
        sipurl += SipUrlParamUser;
        sipurl += userParam;
    }

    // method-param = "method=" Method
    if (methodParam.length() > 0)
    {
        sipurl += SEMICOLON;
        sipurl += SipUrlParamMethod;
        sipurl += methodParam;
    }

    // method-param = "method=" Method
    if (lrParam)
    {
        sipurl += SEMICOLON;
        sipurl += "lr"; // !jf!
    }


    // other-param = ( token | ( token "=" ( token | quoted-string )))
    if (otherParam.length() > 0 )
    {
	sipurl += SEMICOLON;
	sipurl += otherName;
	sipurl += "=";
	sipurl += otherParam;
    } 
    // headers = "?" header *( "&" header )
    // header = hname "=" hvalue
    if(EscObj != 0){
        sipurl += "?";
        sipurl += EscObj->getforwardEscapeheader();
    }

    if (headers.length() > 0) //this should be a list separated by &
    {
        sipurl += "?";
        sipurl += headers;
    }
    if (user.getPtr() != 0)
    {
        if (user->getType() == TEL_USER)
        {
            Sptr <SipTelSubscriberUser> telUser((SipTelSubscriberUser*)(user.getPtr()));
            sipurl += telUser->getTelParams();
        }
    }
    return sipurl;
}    // SipUrl::encode


Data
SipUrl::getNameAddr() const
{
    // return before the url-parameters.
    
    Data nameaddr;
    if (schemeName.length())
    {
	nameaddr = schemeName;
	nameaddr += COLON;
    }

    if (user.getPtr() != 0)
    {
        Data u = user->encode();
        u.removeSpaces();
        if (u.length() > 0) 
        {
            nameaddr += user->encode();
            if (passwd.length() > 0)
            {
                nameaddr += COLON;
                nameaddr += passwd;
            }
            
            nameaddr += "@";
        }
    }

    if(ipv6Addr)
    {
        nameaddr += "[";
        nameaddr += host;
        nameaddr += "]";
    }
    else
    {
        nameaddr += host;
    }

    if ( (port.length()) && port != "0")
    {
        nameaddr += ":";
        nameaddr += port;
    }
    return nameaddr;
}


Data
SipUrl::getUniqueKey() const
{
    return getNameAddr();
}


Data
SipUrl::getNameAddrNoPort() const
{
    // return before the url-parameters.
    Data nameaddr = schemeName;
    Data userValue = getUserValue();

    nameaddr += userValue;

    Data lpasswd = getPasswd();
    if (lpasswd.length() > 0)
    {
        nameaddr += COLON;
        nameaddr += lpasswd;
    }

    if (userValue.length())
    {
        nameaddr += "@";
    }

    if(ipv6Addr)
    {
        nameaddr += "[";
        nameaddr += getHost();
        nameaddr += "]";
    }
    else
    {
        nameaddr += getHost();
    }
    Data myport = getPort();

    if (myport.length() > 0 && (myport != "0") && (myport != "5060"))
    {
        nameaddr += ":";
        nameaddr += myport;
    }
    return nameaddr;
}


Data
SipUrl::getUserValue() const
{
    Data userInfo;
    
    if (user.getPtr() != 0)
    {
	userInfo = user->encode();
    }
    return userInfo;
}



Data
SipUrl::getUrlParam() const
{
    //return only the url parms.
    Data urlparam1;

    if (transportParam.length() )
    {
        urlparam1 += SEMICOLON;
        urlparam1 += TRANSPORT;
	urlparam1 += EQUAL;
        urlparam1 += transportParam;
    }

    if (ttlParam.length() )
    {
        urlparam1 += SEMICOLON;
        urlparam1 += TTL;
	urlparam1 += EQUAL;
        urlparam1 += ttlParam;
    }

    if (maddrParam.length() )
    {
        urlparam1 += SEMICOLON;
        urlparam1 += MADDR_PARM;
	urlparam1 += EQUAL;
        urlparam1 += maddrParam;
    }
    // user-param = "user=" ( "phone" | "ip" )
    if (userParam.length() )
    {
        cpLog(LOG_DEBUG_STACK, "SipUrl::encode : %s", userParam.logData());
        urlparam1 += SEMICOLON;
        urlparam1 += SipUrlParamUser;
        urlparam1 += userParam;
    }

    // method-param = "method=" Method
    if (methodParam.length() )
    {
        urlparam1 += SEMICOLON;
        urlparam1 += SipUrlParamMethod;
        urlparam1 += methodParam;
    }
    // other-param = ( token | ( token "=" ( token | quoted-string )))
    if (otherParam.length() )
    {
        urlparam1 += SEMICOLON;
        urlparam1 += otherName;
        urlparam1 += EQUAL;
        urlparam1 += otherParam;
    }

    if(EscObj != 0){
        urlparam1 += "?";
        urlparam1 += EscObj->getforwardEscapeheader();
    }
    // headers = "?" header *( "&" header )
    // header = hname "=" hvalue
    if (headers.length() > 0)
    {
        urlparam1 += "?";
        urlparam1 += headers;
    }
    
    return urlparam1;
}

void
SipUrl::setTtlParam(const Data& newttl)
{
    Data temp = newttl;

    int myttl = temp.convertInt();

    if ( (myttl < 0 ) || (myttl > 255 ))
    { 
      cpLog(LOG_ERR, "Failed in URL_PARMS(Ttl) Parse'n :~( ");
      throw SipUrlParserException("Failed in URL_PARMS(Ttl) Parse'n :~(", __FILE__, __LINE__, NOT_VALID_TTL_PARM);
    }
    
    else
    {
        ttlParam = newttl;
    }
}

    
bool 
SipUrl::setUserValue(const Data& newUser, const Data& newUserParam)
{
    if (user.getPtr() == 0)
    {
	if ( ( newUserParam == "") || (newUserParam == "ip") )
	{
	    user = new SipUser();
	}
	else if (newUserParam == "phone")
	{
	    user = new SipTelSubscriberUser();
	}
	else
	{
	    //userparam neither blank, ip, nor phone. return false.
	    return false;
	}
    }
    user->decode(newUser);
    userParam = newUserParam;
    return true;
    
}

    
void
SipUrl::setUser(Sptr <BaseUser> newUser)
{
    if (newUser.getPtr() != 0)
    {
        user = newUser->duplicate();
    }
}
    
void 
SipUrl::setEscObj(Sptr<EmbeddedObj> newescobj)
{
    EscObj = newescobj;
}
    
Sptr <BaseUser> 
SipUrl::getUser() const
{
    if (user.getPtr() != 0)
    {
        return user->duplicate();
    }
    else
    {
        return 0; //NULL
    }
}
    
    
const Data&
SipUrl:: getHost() const
{
    return host;
}

    
void
SipUrl::setHost(const Data& newhost)
{
    if(NetworkAddress::is_valid_ip6_addr(newhost))
    {
        ipv6Addr = true;
    }
    else
    {
        ipv6Addr = false;
    }
    host = newhost;
    //assert(strstr(host.c_str(), ":") == NULL);
}

    
const Data&
SipUrl::getPort() const
{
    return port;
}


void SipUrl::setPort(uint16 p) {
    char buf[30];
    snprintf(buf, 30, "%hu", p);
    port = buf;
}
    
void 
SipUrl::setPort(const Data& newport) {
    port = newport;
    char* ptr = NULL;
    strtol(port.c_str(), &ptr, 10);
    assert(!(*ptr));
}

    
const Data&
SipUrl::getPasswd() const
{
    return passwd;
}

    
void 
SipUrl::setPasswd(const Data& newpasswd)
{
    passwd = newpasswd;
}

    
const Data&
SipUrl::getTransportParam() const
{
    return transportParam;
}


void 
SipUrl::setTransportParam(const Data& newtransport)
{
    transportParam = newtransport;
}


const Data& 
SipUrl::getMaddrParam() const
{
    return maddrParam;
}


void
SipUrl::setMaddrParam(const Data& newmaddr)
{
    maddrParam = newmaddr;
}

    
const Data&
SipUrl::getTtlParam() const
{
    return ttlParam;
}

    
const Data& 
SipUrl::getUserParam() const
{
  return userParam;
}
    
    
void
SipUrl::setUserParam(const Data& newUserParam)
{
    userParam = newUserParam;
}

    
const Data& 
SipUrl::getMethodParam() const
{
    return methodParam ;
}

    
void 
SipUrl::setMethodParam(const Data& newmethodParm)
{
    methodParam = newmethodParm;
}

    
const Data& 
SipUrl::getOtherParam() const
{
    return otherParam;
}
    
    
void 
SipUrl::setOtherParam(const Data& newotherParm)
{
    otherParam = newotherParm;
}

    
void 
SipUrl::setOtherName(const Data& newOtherName)
{
    otherName = newOtherName;
}
    
    
const Data&
SipUrl::getOtherName() const
{
    return otherName;
}

bool
SipUrl::isLooseRouterPresent() const
{
    return lrParam;
}

    
const Data& 
SipUrl::getHeaders() const
{
    return headers;
}

    
void 
SipUrl::setHeaders(const Data& newheader)
{
    headers = newheader;
}

bool
SipUrl::parseIpv6Address(Data& inData)
{
    //Look for "]"
    char matchedChar;
    Data aAddr = inData.matchChar("]", &matchedChar);
    if(matchedChar == ']')
    {
        //aAddr is a IPV6 address
        //I think Validation of the address should be done
        //at common place, like in NetworkAddress- SP 
        //So just keep in the raw form
        Data tmp;
        tmp = "[";
        tmp += aAddr;
        tmp += "]";
        //ipv6Addr = true;
        host = tmp;
        return true;
    }
    return false;
}

NetworkAddress
SipUrl::asNetworkAddress() const
{
    // could cache this in the SipUrl !jf!
    return NetworkAddress(host.convertString(), port == "" ? 5060 : port.convertInt());
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
