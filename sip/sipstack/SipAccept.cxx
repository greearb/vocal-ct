
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

static const char* const SipAccept_cxx_Version =
    "$Id: SipAccept.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";



#include "global.h"
#include "symbols.hxx"
#include "SipAccept.hxx"
#include "Data.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;

string
SipAcceptParserException::getName( void ) const
{
    return "SipAcceptParserException";
}

SipAccept::SipAccept(const Data& newaccept, const string& local_ip)
    : SipHeader(local_ip),
        allmedia(false),
        flagmedia(false)
{
    if (newaccept != "") {
        try
        {
            decode(newaccept);
        }
        catch (SipAcceptParserException&)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipAcceptParserException(
                    "failed to decode the Priority string",
                    __FILE__,
                    __LINE__, DECODE_ACCEPT_FAILED);
            }
        }
    }
}


void SipAccept::decode(const Data& acceptdata)
{
    Data data = acceptdata;

    try
    {
        scanAccept(data);
    }
    catch (SipAcceptParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipAcceptParserException(
                "failed to decode the Priority string",
                __FILE__,
                __LINE__, DECODE_ACCEPT_FAILED);
        }
    }
}


void
SipAccept::scanAccept(const Data &tmpdata)
{
    Data accdata;
    Data aptdata = tmpdata;
    int ret = aptdata.match("/", &accdata, true);
    if (ret == FOUND)
    {
        if (accdata == ASTERISK)
        {
            Data acptdata = aptdata;
            Data tkdata;
            int retr = acptdata.match(";", &tkdata, true);
            if (retr == FOUND)
            {
                if (tkdata == ASTERISK)
                {
                    allmedia = true;
                    parseMediaParm(acptdata);
                }
                else
                {
                    if (SipParserMode::sipParserMode())
                    {
                        throw SipAcceptParserException(
                            "failed to decode the Priority string",
                            __FILE__,
                            __LINE__, DECODE_ACCEPT_FAILED);
                    }  //exception
                }

            }
            else if (retr == NOT_FOUND)
            {
                // Since it's optional
                if (acptdata == ASTERISK)
                {
                    allmedia = true;
                }
                else
                {
                    if (SipParserMode::sipParserMode())
                    {

                        throw SipAcceptParserException(
                            "failed to decode the Accept string",
                            __FILE__,
                            __LINE__, DECODE_ACCEPT_FAILED);
                    }  //exceptions because "*/*" is not satisfied
                }

            }
            else if (retr == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw SipAcceptParserException(
                        "failed to decode the Accept string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPT_FAILED);
                }
            }
        }
        else
        {
            setMediaType(accdata);
            Data fdata = aptdata;
            Data kdata ;
            int re = fdata.match(";", &kdata, true);
            if (re == FOUND)
            {
                if (kdata == ASTERISK)
                {
                    flagmedia = true;
                    parseMediaParm(fdata);
                }
                else
                {
                    setMediaSubtype(kdata);

                    string testdata = fdata.convertString();
                    int test1 = testdata.find("q");
                    if (test1 == 0)
                    {
                        parseAcceptParm(fdata);
                    }
                    else
                    {
                        parseMediaParm(fdata);
                    }
                }
            }
            else if (re == NOT_FOUND)
            {
                // Since it's optional
                if (kdata == ASTERISK)
                {
                    flagmedia = true;
                }
                else
                {
                    setMediaSubtype(fdata);
                }
            }
            else if (re == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw SipAcceptParserException(
                        "failed to decode the Accept string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPT_FAILED);
                }   //excpetion
            }

        }
    }
    else if (ret == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipAcceptParserException(
                "failed to decode the Accept string",
                __FILE__,
                __LINE__, DECODE_ACCEPT_FAILED);
        }  //
    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipAcceptParserException(
                "failed to decode the Accept string",
                __FILE__,
                __LINE__, DECODE_ACCEPT_FAILED);
        }
        // //exceptions
    }
}



void
SipAccept::parseMediaParm(const Data& tmpdata)
{
    Data apctdata = tmpdata;
    Data findata;
    string testdata = tmpdata.convertString();
    int test1 = testdata.find("q");
    if (test1 == 0)
    {
        parseAcceptParm(apctdata);
    }
    else
    {
        while (apctdata.length())
        {
            string testdata = apctdata.convertString();
            int test2 = testdata.find("q");
            if (test2 == 0)
            {
                parseAcceptParm(apctdata);
                break;
            }

            int ret = apctdata.match(SEMICOLON, &findata, true);
            if (ret == FOUND)
            {
                parseMediaParms(findata, apctdata);
            }
            else if (ret == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {

                    throw SipAcceptParserException(
                        "failed to decode the Accept string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPT_FAILED);
                }
            }
            else if (ret == NOT_FOUND)
            {
                Data pvalue;
                Data pdata = apctdata;
                int check = pdata.match("=", &pvalue , true);

                if ( check == NOT_FOUND)
                {
                    //Exceptions
                }

                else if (check == FIRST)
                {

                    if (SipParserMode::sipParserMode())
                    {

                        throw SipAcceptParserException(
                            "failed to decode the Accept string",
                            __FILE__,
                            __LINE__, DECODE_ACCEPT_FAILED);
                    }
                }
                else if (check == FOUND)

                    //value has the correct

                    if (pdata.length())
                    {
                        try
                        {
                            parseMediaExtension(pvalue, pdata);
                        }
                        catch (SipAcceptParserException&)
                        {
                            if (SipParserMode::sipParserMode())
                            {

                                throw SipAcceptParserException(
                                    "failed to decode the Accept string",
                                    __FILE__,
                                    __LINE__, DECODE_ACCEPT_FAILED);
                            }


                        }
                    }
                break;

            }

        }
    }
}

void
SipAccept::parseMediaParms(const Data& data, const Data& value)
{

    Data pvalue = value;
    Data pdata = data;
    string testdata = data.convertString();
    int test1 = testdata.find("q");
    if (test1 == 0)
    {
        parseAcceptParm(pdata);
    }

    // Check for the Parms With Constant "=" sign
    int check = pdata.match("=", &pvalue , true);

    if ( check == NOT_FOUND)
    {
        //Exceptions
    }

    else if (check == FIRST)
    {

        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptParserException(
                "failed to decode the Accept string",
                __FILE__,
                __LINE__, DECODE_ACCEPT_FAILED);
        }

    }
    else if (check == FOUND)

        //value has the correct

        if (pdata.length())
        {
            try
            {
                parseMediaExtension(pvalue, pdata);
            }
            catch (SipAcceptParserException&)
            {

                if (SipParserMode::sipParserMode())
                {

                    throw SipAcceptParserException(
                        "failed to decode the Accept string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPT_FAILED);
                }

            }
        }
}


void
SipAccept::parseMediaExtension(const Data& extensiondata, const Data& extensionvalue)
{
    Data mdiatokenparm = extensiondata ;
    Data mdiatokenvalue = extensionvalue;
    setMediaToken(mdiatokenparm);
    setMediaTokenValue(mdiatokenvalue);


}

void
SipAccept::parseAcceptParm(const Data & tempdata)
{
    Data tempvalue;
    Data acparm = tempdata;
    while (acparm.length())
    {
        int ret = acparm.match(SEMICOLON, &tempvalue, true);
        if (ret == FOUND)
        {
            parseAcceptParms(tempvalue);
        }
        else if (ret == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {

                throw SipAcceptParserException(
                    "failed to decode the Accept string",
                    __FILE__,
                    __LINE__, DECODE_ACCEPT_FAILED);
            }
        }
        else if (ret == NOT_FOUND)
        {
            Data pvalue;
            Data pdata = acparm;
            int check = pdata.match("=", &pvalue , true);

            if ( check == NOT_FOUND)
            {
                setMediaToken(pvalue);
            }
            else if (check == FIRST)
            {
                if (SipParserMode::sipParserMode())
                {

                    throw SipAcceptParserException(
                        "failed to decode the Accept string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPT_FAILED);
                }


            }
            else if (check == FOUND)

                //value has the correct

                if (pdata.length())
                {
                    try
                    {
                        parseAcceptExtension(pvalue, pdata);
                    }
                    catch (SipAcceptParserException&)
                    {
                        if (SipParserMode::sipParserMode())
                        {

                            throw SipAcceptParserException(
                                "failed to decode the Accept  string",
                                __FILE__,
                                __LINE__, DECODE_ACCEPT_FAILED);
                        }


                    }
                }
            break;

        }

    }
}
void
SipAccept::parseAcceptParms(const Data& data)
{

    Data pvalue;
    Data pdata = data;
    // Check for the Parms With Constant "=" sign
    int check = pdata.match("=", &pvalue , true);

    if ( check == NOT_FOUND)
    {
        setAcceptExtensionParm(pvalue);
    }
    else if (check == FOUND)
    {
        parseAcceptExtension(pvalue, pdata);
    }
    else //if (check == FIRST)
    {
        //do nothing
    }
}



void
SipAccept::parseAcceptExtension(const Data& extensiondata, const Data& extensionvalue)
{
    if (extensiondata == QVALUE)
    {
        qValue = extensionvalue;
    }
    else
    {
        setAcceptExtensionParm(extensiondata);
        setAcceptExtensionValue(extensionvalue);
    }

}

const SipAccept& SipAccept::operator =(const SipAccept& src)
{
    if (&src != this)
    {
        mediaType = src.mediaType;
        mediaSubtype = src.mediaSubtype;
        qValue = src.qValue;
        mediatoken = src.mediatoken;
        mediatokenValue = src.mediatokenValue;
        accepttoken = src.accepttoken;
        accepttokenValue = src.accepttokenValue;
        allmedia = src.allmedia;
        flagmedia = src.flagmedia;
    }
    return (*this);
}

bool SipAccept::operator == (const SipAccept& src) const
{
    if (
        (mediaType == src.mediaType) &&
        (mediaSubtype == src.mediaSubtype) &&
        (qValue == src.qValue) &&
        (mediatoken == src.mediatoken) &&
        (mediatokenValue == src.mediatokenValue) &&
        (accepttoken == src.accepttoken) &&
        (accepttokenValue == src.accepttokenValue) &&
        (allmedia == src.allmedia) &&
        (flagmedia == src.flagmedia)
    )
    {
        return true;
    }
    else
    {
        return false;
    }
}

SipAccept::SipAccept( const SipAccept& src )
        : SipHeader(src),
          mediaType(src.mediaType),
        mediaSubtype(src.mediaSubtype),
        qValue(src.qValue),
        mediatoken(src.mediatoken),
        mediatokenValue(src.mediatokenValue),
        accepttoken(src.accepttoken),
        accepttokenValue(src.accepttokenValue),
        allmedia(src.allmedia),
        flagmedia(src.flagmedia)
{}


Data 
SipAccept::getMediaType() const
{
    return mediaType;
}

void 
SipAccept::setMediaType(const Data& srcmediaType)
{
    mediaType = srcmediaType;
    //when mediaType is set, the subTYpe is set as *.
    mediaSubtype = ALL_MEDIASUBTYPES;
}

Data 
SipAccept::getMediaSubtype() const
{
    return mediaSubtype;
}

void 
SipAccept::setMediaSubtype(const Data& srcmediaSubtype)
{
    mediaSubtype = srcmediaSubtype;
}

Data 
SipAccept::getqValue() const
{
    return qValue;
}

void 
SipAccept::setqValue(const Data& srcqValue)
{
    qValue = srcqValue;
}

Data 
SipAccept::getMediaToken() const
{
    return mediatoken;
}

void 
SipAccept::setMediaToken(const Data& srcToken)
{
    mediatoken = srcToken;
}

Data 
SipAccept::getMediaTokenValue() const
{
    return mediatokenValue;
}

void 
SipAccept::setMediaTokenValue(const Data& srcTokenValue)
{
    mediatokenValue = srcTokenValue;
}


void 
SipAccept::setAcceptExtensionParm(const Data& newaccepttoken)
{
    accepttoken = newaccepttoken;
}


void 
SipAccept::setAcceptExtensionValue(const Data& newaccepttokenvalue)
{
    accepttokenValue = newaccepttokenvalue;
}


Data 
SipAccept::getAccpetExtensionParm() const
{
    return accepttoken;
}


Data 
SipAccept::getAccpetExtensionValue() const
{
    return accepttokenValue;
}


/*** return the encoded string version of this. This call should only be
     used inside the stack and is not part of the API */
Data 
SipAccept::encode() const
{
    Data data;
    if (allmedia)
    {
        data = "*";
        data += "/";
        data += "*";
    }
    if (mediaType.length())
    {
        data = mediaType;
        data += "/";
        data += mediaSubtype;
    }

    if ( (mediatoken.length()) && (mediatokenValue.length()) )
    {
        data += SEMICOLON;
        data += mediatoken;
        data += "=";
        data += mediatokenValue;
    }

    if (qValue.length())
    {
        data += SEMICOLON;
        data += "q=";
        data += qValue;
    }

    if ((accepttoken.length()) && (accepttokenValue.length() == 0) )
    {
        data += SEMICOLON;
        data += accepttoken;

    }
    if ((accepttoken.length()) && (accepttokenValue.length()))
    {
        data += SEMICOLON;
        data += accepttoken;
        data += "=";
        data += accepttokenValue;
    }
    return data;
}






SipHeader*
SipAccept::duplicate() const
{
    return new SipAccept(*this);
}


bool
SipAccept::compareSipHeader(SipHeader* msg) const
{
    SipAccept* otherMsg = dynamic_cast<SipAccept*>(msg);
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
