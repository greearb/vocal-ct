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
    "$Id: SipRedirect.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipRedirect.hxx"
#include "symbols.hxx"



string
SipRedirectParserException::getName( void ) const
{
    return "SipRedirectParserException";
}


SipRedirect::SipRedirect()
        : rurl(),
        tokenMap()
{
    rurl.initializeFromTo();
}
///
SipRedirect::SipRedirect( const SipUrl& rurl):
        rurl(rurl),
        tokenMap()
{
}
///

SipRedirect::~SipRedirect()
{
}
///

SipRedirect::SipRedirect(const Data& data )
        : rurl(),
        tokenMap()
{
    rurl.initializeFromTo();
    try
    {
        Data kdata = data;
        decode(kdata);
    }
    catch (SipRedirectParserException& expection)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, DECODE_REDIRECT_FAILED);

        }

    }
}
///
SipRedirect::SipRedirect(const SipRedirect& sredirect)
{
    rurl = sredirect.rurl;
    rcounter = sredirect.rcounter;
    rlimit = sredirect.rlimit;
    rreason = sredirect.rreason;
    displayName = sredirect.displayName;
    tag = sredirect.tag;
    tokenMap = sredirect.tokenMap;
}
///
Data SipRedirect::encode()
{
    Data sipRedirect;

    sipRedirect = SIPREDIRECT;
    sipRedirect += SP;

    if (displayName.length() )
    {
        sipRedirect += displayName;
    }

    sipRedirect += "<";

    rurl.encode();

    //get before the url param.
    Data nameaddr;
    nameaddr = rurl.getNameAddr();
    sipRedirect += nameaddr;

    rurl.setTransport("");
    rurl.setMaddr("");

    Data userparam;
    userparam = rurl.getUrlParam();

    if (userparam.length())
    {
        sipRedirect += userparam;
    }
    sipRedirect += ">";


    Data tagparam = getTag();
    if (tagparam.length() > 0)
    {
        sipRedirect += Data(";") + "tag=";
        sipRedirect += tagparam;
    }
    if (tokenMap.size() != 0 )
    {
        sipRedirect += ";";
    }
    TokenMapRedirect::iterator i = tokenMap.begin();
    while (i != tokenMap.end())
    {
        Data token1 = i->first;
        Data tokenValue1 = i->second;
        sipRedirect += token1;
        if (tokenValue1.length() > 0)
        {
            sipRedirect += "=";
            sipRedirect += tokenValue1;
        }

        ++i;

        if ( i != tokenMap.end())
        {
            sipRedirect += ";";
        }
    }

    Data reasonparam = getReason();
    LocalScopeAllocator lo;
    string reasons = reasonparam.getData(lo);
    if (reasonparam.length() > 0)
    {
        sipRedirect += Data(";") + REASON;
        sipRedirect += reasonparam;
    }

    Data countparam = getCounter();

    string counts = countparam.getData(lo);

    if (countparam.length() > 0)
    {
        sipRedirect += Data(";") + COUNTER;
        sipRedirect += countparam;
    }

    Data limitparam = getLimit();
    string limits = limitparam.getData(lo);
    if (limitparam.length() > 0)
    {
        sipRedirect += Data(";") + LIMIT;
        sipRedirect += limitparam;
    }


    sipRedirect += CRLF;

    return sipRedirect;


}

void SipRedirect::decode(const Data& redirectdata)
{
    Data rdata = redirectdata;
    try
    {
        parse(rdata);
    }
    catch (SipRedirectParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");
            throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, DECODE_REDIRECT_FAILED);

        }



    }

}
///
SipRedirect& SipRedirect::operator = (const SipRedirect& sredirect)
{
    if (&sredirect != this)
    {
        rurl = sredirect.rurl;
        rcounter = sredirect.rcounter;
        rlimit = sredirect.rlimit;
        rreason = sredirect.rreason;
        displayName = sredirect.displayName;
        tag = sredirect.tag;
        tokenMap = sredirect.tokenMap;
    }
    return *this;
}
///
void SipRedirect::setReason(const Data& res)
{

    rreason = res;
}
///
Data SipRedirect::getReason() const
{
    return rreason;
}

///
void SipRedirect::setCounter(const Data& count)
{

    rcounter = count;
}
///
Data SipRedirect::getCounter() const
{
    return rcounter;
}

///
void SipRedirect::setLimit(const Data& lim)
{

    rlimit = lim;
}
///
Data SipRedirect::getLimit() const
{
    return rlimit;
}
///

SipUrl SipRedirect::getUrl() const
{
    return rurl;
}
///
void SipRedirect::setUrl(const SipUrl& redirecturl)
{
    rurl = redirecturl;
}
///
bool SipRedirect::operator ==(const SipRedirect& sredirect) const
{
    cpLog(LOG_DEBUG_STACK, "SipRedirect ::operator ==");
    bool equal = (( rurl == sredirect.rurl) &&
                  ( rcounter == sredirect.rcounter) &&
                  ( rlimit == sredirect.rlimit) &&
                  ( rreason == sredirect.rreason));
    bool sel = (rurl == sredirect.rurl);
    cpLog(LOG_DEBUG_STACK, " url equal is : %d", sel);

    sel = sel && ( rcounter == sredirect.rcounter);
    cpLog(LOG_DEBUG_STACK, " counter1 equal is : %d", sel);

    sel = sel && ( rlimit == sredirect.rlimit);
    cpLog(LOG_DEBUG_STACK, " limit1 equal is : %d", sel);

    sel = sel && ( rreason == sredirect.rreason);
    cpLog(LOG_DEBUG_STACK, " reason1 equal is : %d", sel);


    return equal;
}



void
SipRedirect::parse( const Data& redirectdata)
{

    Data data = redirectdata;

    try
    {

        scanSipRedirect(data);
    }
    catch (SipRedirectParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");
            throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, DECODE_REDIRECT_FAILED);

        }

    }


    //everything allright.


}




void
SipRedirect::scanSipRedirect( const Data & tmpdata)
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
            SipUrl tUrl(data);
            rurl = tUrl;
        }
        if (retn == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
                throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, DECODE_REDIRECT_FAILED);

            }

        }
        if (retn == FOUND)
        { // if found then it has the Addrs-params
            tmpval = value;
            SipUrl oUrl(tmpval);
            rurl = oUrl;
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

void SipRedirect::setHost(const Data& newhost)
{
    rurl.setHost(newhost);
}

Data SipRedirect::getHost() const
{
    return rurl.getHost();
}



void
SipRedirect::parseNameInfo(const Data& data)
{
    Data newnameinfo;
    Data nameinfo = data;
    int ret = nameinfo.match(":", &newnameinfo, true);
    Data newnameinf;
    ret = nameinfo.match(" ", &newnameinf, true);
    setDisplayName(nameinfo);
}



void SipRedirect::setDisplayName(const Data& name)
{
    displayName = name;
}



void
SipRedirect::parseUrl(const Data& data)
{
    Data urlvalue = data;
    Data avalue;

    int retur = urlvalue.match(">", &avalue, true);
    if (retur == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl() :o( ");
            throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, PARSE_RURL_FAILED);

        }


    }
    if (retur == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in ParseUrl()  :o( ");
            throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, PARSE_RURL_FAILED);

        }

    }
    if (retur == FOUND)
    {

        SipUrl Url(avalue);

        rurl = Url;
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
        {}

    }
}


void
SipRedirect::parseToken(const Data& data)
{
    int i = 0;
    Data resdata;
    Data edata = data;
    while (i == 0)
    {
        resdata = isToken(edata);
        if ( resdata != edata)
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
SipRedirect::parseTag( Data& data)
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
SipRedirect::parseReason(const Data& data)
{
    Data addrparm = data;
    Data parm;
    int ret = addrparm.match("=", &parm, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseLimit:o( ");
            throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, LIMIT_ERR);

        }
    }
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseLimit :o( ");
            throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, LIMIT_ERR);

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
        }
        if (rety == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Parse in parseLimit :o( ");
                throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, LIMIT_ERR);
            }
        }
        if (rety == NOT_FOUND)
        {}


    }
}



void
SipRedirect::parseCounter(const Data& data)
{
    Data addrparm = data;
    Data parm;

    int ret = addrparm.match("=", &parm, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseCounter :o( ");
            throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, COUNT_FAILED);

        }
    }
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseCounter :o( ");
            throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, COUNT_FAILED);

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
        }
        if (rety == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in ParseCounter :o( ");
                throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, COUNT_FAILED);

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
SipRedirect::parseLimit(const Data& data)
{
    Data addrparm = data;
    Data parm;
    int ret = addrparm.match("=", &parm, true);
    if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseReason:o( ");
            throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, LIMIT_ERR);

        }
    }
    if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Parse in parseReason :o( ");
            throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, LIMIT_ERR);

        }

    }
    if (ret == FOUND)
    {

        Data te = addrparm;

        Data fik;

        int rety = te.match(";", &fik, true);

        if (rety == FOUND)
        {
        }

        if (rety == FIRST)
        {

            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in ParseReason  :o( ");
                throw SipRedirectParserException("failed in Decode", __FILE__, __LINE__, REASON_FAILED);

            }
        }
        if (rety == NOT_FOUND)
        {
            setLimit(te);
        }

    }
}



bool
SipRedirect::operator< (const SipRedirect& src) const
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


void SipRedirect::setTokenDetails(const Data& token, const Data& tokenValue)
{
    tokenMap[token] = tokenValue;
}


Sptr < SipRedirect::TokenMapRedirect >
SipRedirect::getTokenDetails()
{
    Sptr < SipRedirect::TokenMapRedirect > dmap = new TokenMapRedirect(tokenMap) ;

    return dmap;
}


void SipRedirect::setTag(const Data& newtag)
{
    tag = newtag;
}


Data SipRedirect::getTag() const
{
    return tag;
}


//is the string a token  or not
Data SipRedirect::isToken(const Data& sdata )
{
    Data vdata;
    Data mdata("OVER");
    Data ldata = sdata;
    if ( ! isEmpty(ldata))
    {
        //	if ((!(isReason(ldata))) && ( !(isCounter(ldata))) && (!(isLimit(ldata))))
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
                        {}


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

bool SipRedirect::isCounter(const Data& sdata)
{
    if ( ! isEmpty(sdata))
    {
        Data data = sdata;
        Data value;
        int retn = data.match("=", &value, true) ;

        if (retn == FOUND)
        {
            if (value == "redir-counter")
            {
                return true;
            }
            return false;
        }

    }

    return false;
}

bool SipRedirect::isLimit(const Data& sdata)
{
    if ( ! isEmpty(sdata))
    {
        Data data = sdata;
        Data value;
        int retn = data.match("=", &value, true) ;

        if (retn == FOUND)
        {
            if (value == "redir-limit")
            {
                return true;
            }
            return false;
        }

    }

    return false;

}

//is it a reason string or not
bool SipRedirect::isReason(const Data& sdata)
{
    if ( ! isEmpty(sdata))
    {
        Data data = sdata;
        Data value;
        int retn = data.match("=", &value, true) ;

        if (retn == FOUND)
        {
            if (value == "redir-reason")
            {
                return true;
            }
            return false;
        }

    }
    return false;
}

//function checks whether the data string is empty or not
bool SipRedirect::isEmpty(const Data& sdata)
{
    Data data = sdata;
    if ( data.length() == 0)
    {
        return true;
    }
    return false;
}


void SipRedirect::print()
{
    TokenMapRedirect::iterator i = tokenMap.begin();
    while (i != tokenMap.end())
    {
        Data token1 = i->first;
        Data tokenValue1 = i->second;
        cpLog(LOG_DEBUG_STACK, "token1: %s", token1.logData());
        cpLog(LOG_DEBUG_STACK, "tokenValue1: %s", tokenValue1.logData());
        ++i;


    }

}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
