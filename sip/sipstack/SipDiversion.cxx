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

static const char* const SipRedirect_cxx_Version =
    "$Id: SipDiversion.cxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include "global.h"
#include "cpLog.h"
#include "symbols.hxx"
#include "SipDiversion.hxx"
#include "SipParserMode.hxx"
#include "SipUrl.hxx"

using namespace Vocal;


string
SipDiversionParserException::getName( void ) const
{
    return "SipDiversionParserException";
}


SipDiversion::SipDiversion( Sptr <BaseUrl> rurl, const string& local_ip)
    : SipHeader(local_ip),
      rurl(duplicateUrl(rurl)),
      tokenMap()
{
}


SipDiversion::~SipDiversion()
{
}


SipDiversion::SipDiversion(const Data& data, const string& local_ip,
                           UrlType uType)
    : SipHeader(local_ip),
      rurl(),
      tokenMap()
{
    if (data == "") {
        urlType =  uType;
        return;
    }

    try
    {
        Data kdata = data;
        decode(kdata);
	if (rurl != 0)
	{
	    urlType = rurl->getType();
    
	    if (rurl->getType() == SIP_URL)
	    {
		Sptr <SipUrl> sipUrl((SipUrl*)(rurl.getPtr()));
		sipUrl->initializeFrom();
		sipUrl->initializeTo();
	    }
	}
    }
    catch (SipDiversionParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, DECODE_DIVERSION_FAILED);

        }

    }
}


SipDiversion::SipDiversion(const SipDiversion& src)
    : SipHeader(src),
      rurl(duplicateUrl(src.rurl))
{
    if (rurl != 0)
    {
	urlType = rurl->getType();
    
	if (rurl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(rurl.getPtr()));
    
	    sipUrl->initializeTo();
	    sipUrl->initializeFrom();
	}
    }
    rcounter = src.rcounter;
    rlimit = src.rlimit;
    rreason = src.rreason;
    displayName = src.displayName;
    tag = src.tag;
    tokenMap = src.tokenMap;
}

Data SipDiversion::encode() const
{
    Data sipDiversion;

    sipDiversion = SIPDIVERSION;
    sipDiversion += SP;

    if (displayName.length() )
    {
        sipDiversion += displayName;
    }

    if (rurl != 0)
    {
	if (rurl->getType() == SIP_URL)
	{
	    rurl->encode();
	    
	    sipDiversion += "<";
    
            Sptr <SipUrl> sipUrl((SipUrl*)(rurl.getPtr()));
    
	    //get before the url param.
	    Data nameaddr;
	    nameaddr = sipUrl->getNameAddr();
	    sipDiversion += nameaddr;
	    
	    Data userparam;
	    userparam = sipUrl->getUrlParam();
	     if (userparam.length())
	     {
		 sipDiversion += userparam;
	     }
	     sipDiversion += ">";
	}
    }

   


    Data tagparam = getTag();
    if (tagparam.length() > 0)
    {
        sipDiversion += Data(";") + "tag=";
        sipDiversion += tagparam;
    }
    if (tokenMap.size() != 0 )
    {
        sipDiversion += ";";
    }
    TokenMapDiversion::const_iterator i = tokenMap.begin();
    while (i != tokenMap.end())
    {
        Data token1 = i->first;
        Data tokenValue1 = i->second;
        sipDiversion += token1;
        if (tokenValue1.length() > 0)
        {
            sipDiversion += "=";
            sipDiversion += tokenValue1;
        }

        ++i;

        if ( i != tokenMap.end())
        {
            sipDiversion += ";";
        }
    }

    Data reasonparam = getReason();
    LocalScopeAllocator lo;
    string reasons = reasonparam.getData(lo);
    if (reasonparam.length() > 0)
    {
        sipDiversion += Data(";") + DIVERSION_REASON;
        sipDiversion += reasonparam;
    }



    Data countparam = getCounter();

    string counts = countparam.getData(lo);

    if (countparam.length() > 0)
    {
        sipDiversion += Data(";") + COUNTER;
        sipDiversion += countparam;
    }

    Data limitparam = getLimit();
    string limits = limitparam.getData(lo);
    if (limitparam.length() > 0)
    {
        sipDiversion += Data(";") + LIMIT;
        sipDiversion += limitparam;
    }


    sipDiversion += CRLF;

    return sipDiversion;


}

void SipDiversion::decode(const Data& diversiondata)
{
    Data rdata = diversiondata;
    try
    {
        parse(rdata);
    }
    catch (SipDiversionParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");
            throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, DECODE_DIVERSION_FAILED);

        }



    }

}


SipDiversion& 
SipDiversion::operator = (const SipDiversion& src)
{
    if (&src != this)
    {
        rurl = duplicateUrl(src.rurl);
	urlType = src.urlType;
        rcounter = src.rcounter;
        rlimit = src.rlimit;
        rreason = src.rreason;
        displayName = src.displayName;
        tag = src.tag;
        tokenMap = src.tokenMap;
    }
    return *this;
}


void 
SipDiversion::setReason(const Data& res)
{

    rreason = res;
}


Data 
SipDiversion::getReason() const
{
    return rreason;
}


void 
SipDiversion::setCounter(const Data& count)
{

    rcounter = count;
}


Data 
SipDiversion::getCounter() const
{
    return rcounter;
}


void 
SipDiversion::setLimit(const Data& lim)
{

    rlimit = lim;
}


Data 
SipDiversion::getLimit() const
{
    return rlimit;
}


Sptr<BaseUrl> 
SipDiversion::getUrl() const
{
    return duplicateUrl(rurl, true);
}


void 
SipDiversion::setUrl(Sptr<BaseUrl> diversionurl)
{
    rurl = duplicateUrl(diversionurl);
}


bool 
SipDiversion::operator ==(const SipDiversion& sdiversion) const
{
    bool equal = false;
    
    cpLog(LOG_DEBUG_STACK, "SipDiversion ::operator ==");
    if ( (rurl != 0) && (sdiversion.rurl != 0) )
    {
	equal = (rurl->areEqual(sdiversion.rurl));
    }
    else if ( (rurl == 0) && (sdiversion.rurl == 0) ) 
    {
	equal = true;
    }
    else
    {
	equal = false;
    }
    cpLog(LOG_DEBUG_STACK, " url equal is : %d", equal);
    
    equal = equal && (( rcounter == sdiversion.rcounter) &&
                  ( rlimit == sdiversion.rlimit) &&
                  ( rreason == sdiversion.rreason));
   
    return equal;
}


void
SipDiversion::parse( const Data& diversiondata)
{

    Data data = diversiondata;

    try
    {

        scanSipDiversion(data);
    }
    catch (SipDiversionParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");
            throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, DECODE_DIVERSION_FAILED);

        }

    }
    //everything allright.
}


void
SipDiversion::scanSipDiversion( const Data & tmpdata)
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
            rurl = BaseUrl::decode(data, getLocalIp());
        }
        if (retn == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, DECODE_DIVERSION_FAILED);

            }

        }
        if (retn == FOUND)
        { // if found then it has the Addrs-params
            tmpval = value;
            rurl = BaseUrl::decode(tmpval, getLocalIp());
            value = data;
        }

    }
    if (ret == FIRST)
    {
        parseUrl(data);
    }

    if (ret == FOUND)
    {


        if (sipdata.length())
        {
            parseNameInfo(sipdata);
        }
        Data urlvalue;
        Data tempData = data;

        parseUrl(data);
    }
}


void 
SipDiversion::setHost(const Data& newhost)
{
    if (urlType == TEL_URL)
    {
	return;
    }
    if (rurl == 0)
    {
	rurl = new SipUrl("", getLocalIp());
    }
    if (rurl != 0)
    {
	if (rurl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl((SipUrl*)(rurl.getPtr()));
    
	    sipUrl->setHost(newhost);
	}
    }
}


Data 
SipDiversion::getHost() const
{
    Data myHost;
    if (rurl != 0)
    {
	if (rurl->getType() == SIP_URL)
	{
	    Sptr<SipUrl> sipUrl((SipUrl*)(rurl.getPtr()));
    
	    myHost = sipUrl->getHost();
	}
    }
    return myHost;
}


void
SipDiversion::parseNameInfo(const Data& data)
{
    Data newnameinfo;
    Data nameinfo = data;
    int ret = nameinfo.match(":", &newnameinfo, true);
    Data newnameinf;
    ret = nameinfo.match(" ", &newnameinf, true);
    setDisplayName(nameinfo);
}


void 
SipDiversion::setDisplayName(const Data& name)
{
    displayName = name;
}


void
SipDiversion::parseUrl(const Data& data)
{
    Data urlvalue = data;
    Data avalue;

    int retur = urlvalue.match(">", &avalue, true);
    if (retur == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl() :o( ");
            throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, PARSE_RURL_FAILED);

        }
    }
    if (retur == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl()  :o( ");
            throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, PARSE_RURL_FAILED);

        }
    }
    if (retur == FOUND)
    {

        rurl = BaseUrl::decode(avalue, getLocalIp());

        Data te = urlvalue;

        Data fik;
        int rety = te.match(";", &fik, true);

        if (rety == FOUND)
        {
        }

        if (rety == FIRST)
        {
            parseTag(te);
        }
        if (rety == NOT_FOUND)
        {
        }
    }
}


void
SipDiversion::parseToken(const Data& data)
{
    int i = 0;
    Data resdata;
    Data edata = data;
    
    while (i == 0)
    {
        resdata = isToken(edata);
	
        if ( resdata.operator!=(edata))
        {

            edata = resdata;
        }
        else
        {
            i = 1;
        }

    }
    if (isReason(edata))
    {
        parseReason(edata);
    }
    else
        if ( isCounter(edata))
        {
            parseCounter(edata);
        }
        else if ( isLimit(edata))
        {
            parseLimit(edata);
        }
}


void
SipDiversion::parseTag( Data& data)
{

    Data addrparm = data;
    Data parm;
    int ret = addrparm.match("=", &parm, true);
    if (ret == NOT_FOUND)
    {
        // if addrparm != 0 exception
    }

    if (ret == FIRST)
    {
        //exception
    }

    if (ret == FOUND)
    {
        //parse tokens
        //parse reason and rest of the stuff

        if (!isEmpty(parm))
        {
            Data value;
            if (parm == "tag")
            {
                Data parma;
                int sret = addrparm.match(";", &parma, true);
                if (sret == FOUND)
                {
                    setTag(parma);
                    parseToken(addrparm);
                }
            }
            else
            {
                parseToken(data);
            }
        }
    }
}


void
SipDiversion::parseReason(const Data& data)
{
    Data addrparm = data;
    Data parm;
    int ret = addrparm.match("=", &parm, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseLimit:o( ");
            throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, LIMIT_ERR);

        }
    }
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseLimit :o( ");
            throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, LIMIT_ERR);

        }
    }
    if (ret == FOUND)
    {

        Data te = addrparm;
        Data fik;

        int rety = te.match(";", &fik, true);

        if (rety == FOUND)
        {
            setReason(fik);
            if (isCounter(te))
            {
                parseCounter(te);
            }

            else if (isLimit(te))
            {
                parseLimit(te);
            }
	    // else { diversion extension }
        }
        if (rety == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Parse in parseLimit :o( ");
                throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, LIMIT_ERR);
            }
        }
        if (rety == NOT_FOUND)
        {
        }
    }
}


void
SipDiversion::parseCounter(const Data& data)
{
    Data addrparm = data;
    Data parm;

    int ret = addrparm.match("=", &parm, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseCounter :o( ");
            throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, COUNT_FAILED);

        }
    }
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseCounter :o( ");
            throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, COUNT_FAILED);

        }
    }
    if (ret == FOUND)
    {

        Data te = addrparm;

        Data fik;

        int rety = te.match(";", &fik, true);

        if (rety == FOUND)
        {
            setCounter(fik);
            if ( isLimit(te))
            {
                parseLimit(te);
            }
	    // else { diversion extension }
        }
        if (rety == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in ParseCounter :o( ");
                throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, COUNT_FAILED);

            }
        }
        if (rety == NOT_FOUND)
        {
            if (isEmpty(fik))
            {
                setCounter(te);
            }
        }

    }
}


void
SipDiversion::parseLimit(const Data& data)
{
    Data addrparm = data;
    Data parm;
    int ret = addrparm.match("=", &parm, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseReason:o( ");
            throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, LIMIT_ERR);
        }
    }
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseReason :o( ");
            throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, LIMIT_ERR);

        }
    }
    if (ret == FOUND)
    {

        Data te = addrparm;
        Data fik;
        int rety = te.match(";", &fik, true);

        if (rety == FOUND)
        {
	    //parse extension
        }

        if (rety == FIRST)
        {

            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in ParseReason  :o( ");
                throw SipDiversionParserException("failed in Decode", __FILE__, __LINE__, REASON_FAILED);

            }
        }
        if (rety == NOT_FOUND)
        {
            setLimit(te);
	    //parseextension
        }
    }
}


bool
SipDiversion::operator< (const SipDiversion& src) const
{
    if ( //(host == src.host) &&
        (rcounter < src.rcounter) &&
        (rlimit < src.rlimit) &&
        (rreason < src.rreason)
    )
    {
        return true;
    }
    return false;
}


void 
SipDiversion::setTokenDetails(const Data& token, const Data& tokenValue)
{
    tokenMap[token] = tokenValue;
}


/** Calling code better clean this memory up! */
SipDiversion::TokenMapDiversion*
SipDiversion::createTokenDetails() {
    return new TokenMapDiversion(tokenMap);
}


void 
SipDiversion::setTag(const Data& newtag)
{
    tag = newtag;
}


Data 
SipDiversion::getTag() const
{
    return tag;
}


//is the string a token  or not
Data 
SipDiversion::isToken(const Data& sdata )
{
    Data vdata;
    Data mdata("OVER");
    Data ldata = sdata;
    if ( ! isEmpty(ldata))
    {
        if ( isReason(ldata) == false)
        {
            if ( isCounter(ldata) == false)
            {
                if ( isLimit(ldata) == false)
                {
                    Data data = ldata;
                    Data value;
                    int retn = data.match("=", &value, true) ;
                    if (retn == FOUND)
                        //if (!isEmpty(&data))
                    {
                        Data dat;

                        Data gdata = data;
                        int ret = gdata.match(";", &dat, true) ;
                        if ( ret == FOUND)
                            //    if ((!isEmpty(&dat)) && (ret !=0))
                        {
                            tokenMap[value] = dat;
                            return gdata;
                        }
                        else if (ret == NOT_FOUND)
                        {
                            tokenMap[value] = data;
                            return vdata;
                        }
                        else if (ret == FIRST)
                        {
                        }
                    }
                    if ( retn == NOT_FOUND)
                    {
                        return ldata;
                    }
                }
            }
        }
        return ldata;
    }

    return vdata;
}


bool 
SipDiversion::isCounter(const Data& sdata)
{
    if ( ! isEmpty(sdata))
    {
        Data data = sdata;
        Data value;
        int retn = data.match("=", &value, true) ;

        if (retn == FOUND)
        {
            if (value == "counter")
            {
                return true;
            }
            return false;
        }
    }

    return false;
}


bool 
SipDiversion::isLimit(const Data& sdata)
{
    if ( ! isEmpty(sdata))
    {
        Data data = sdata;
        Data value;
        int retn = data.match("=", &value, true) ;

        if (retn == FOUND)
        {
            if (value == "limit")
            {
                return true;
            }
            return false;
        }

    }

    return false;

}


bool 
SipDiversion::isReason(const Data& sdata)
{
    if ( ! isEmpty(sdata))
    {
        Data data = sdata;
        Data value;
        int retn = data.match("=", &value, true) ;

        if (retn == FOUND)
        {
            if (value == "reason")
            { 
                return true;
            }
            return false;
        }

    }
    return false;
}


bool 
SipDiversion::isEmpty(const Data& sdata)
{
    Data data = sdata;
    if ( data.length() == 0)
    {
        return true;
    }
    return false;
}


void 
SipDiversion::print()
{
    TokenMapDiversion::iterator i = tokenMap.begin();
    while (i != tokenMap.end())
    {
        Data token1 = i->first;
        Data tokenValue1 = i->second;
        cpLog(LOG_DEBUG_STACK, "token1: %s", token1.logData());
        cpLog(LOG_DEBUG_STACK, "tokenValue1: %s", tokenValue1.logData());
        ++i;


    }

}


SipHeader*
SipDiversion::duplicate() const
{
    return new SipDiversion(*this);
}


bool
SipDiversion::compareSipHeader(SipHeader* msg) const
{
    SipDiversion* otherMsg = dynamic_cast<SipDiversion*>(msg);
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
