
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

static const char* const SipAcceptLanguage_cxx_Version =
    "$Id: SipAcceptLanguage.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


#include "global.h"
#include "SipAcceptLanguage.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipAcceptLanguageParserException::getName( void ) const
{
    return "SipAcceptLanguageParserException";
}


SipAcceptLanguage::SipAcceptLanguage( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip)
{
    if (srcData == "") {
        return;
    }

    try
    {
        decode(srcData);
    }
    catch (SipAcceptLanguageParserException&)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptLanguageParserException(
                "failed to decode theAccept Language string",
                __FILE__,
                __LINE__, DECODE_ACCEPTLANG_FAILED);
        }
    }
}


void SipAcceptLanguage::decode(const Data& vdata)
{
    try
    {

        scanAcceptLanguage(vdata);
    }
    catch (SipAcceptLanguageParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptLanguageParserException(
                "failed to decode theAccept Language string",
                __FILE__,
                __LINE__, DECODE_ACCEPTLANG_FAILED);
        }
    }

}


void
SipAcceptLanguage::scanAcceptLanguage(const Data & scandata)

{
    Data aedata = scandata;
    Data aevalue;
    int ret = aedata.match(";", &aevalue, true);
    if (ret == FOUND)
    {
        parseRange(aevalue);
        Data qdata = aedata;
        Data qval;
        int retn = qdata.match("=", &qval, true);
        if (retn == FOUND)
        {
            if (qval == QVALUE)
            {
                setqValue(qdata);
            }
            else
            {

                if (SipParserMode::sipParserMode())
                {

                    throw SipAcceptLanguageParserException(
                        "failed to decode theAccept Language string",
                        __FILE__,
                        __LINE__, DECODE_ACCEPTLANG_FAILED);
                }

            }
        }
        else if (retn == NOT_FOUND)
        {

            if (SipParserMode::sipParserMode())
            {

                throw SipAcceptLanguageParserException(
                    "failed to decode theAccept Language string",
                    __FILE__,
                    __LINE__, DECODE_ACCEPTLANG_FAILED);
            }

        }
        else if (retn == FIRST)
        {

            if (SipParserMode::sipParserMode())
            {

                throw SipAcceptLanguageParserException(
                    "failed to decode theAccept Language string",
                    __FILE__,
                    __LINE__, DECODE_ACCEPTLANG_FAILED);
            }

        }
    }

    else if (ret == NOT_FOUND)
    {
        parseRange(aevalue);
    }
    else if (ret == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptLanguageParserException(
                "failed to decode theAccept Language string",
                __FILE__,
                __LINE__, DECODE_ACCEPTLANG_FAILED);
        }
    }
}


void
SipAcceptLanguage::parseRange( const Data& newdata )
{
    Data rangedata = newdata;
    Data rangeval;
    int ret = rangedata.match("-", &rangeval, true);
    if (ret == FOUND)
    {
        setPrimaryTag(rangeval);
        setSubTag(rangedata);
    }
    else if ( ret == NOT_FOUND)
    {
        setPrimaryTag(rangeval);
    }
    else if (ret == FIRST)
    {

        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptLanguageParserException(
                "failed to decode theAccept Language string",
                __FILE__,
                __LINE__, DECODE_ACCEPTLANG_FAILED);
        }
    }
}


SipAcceptLanguage::SipAcceptLanguage( const SipAcceptLanguage& src)
        : SipHeader(src),
          primaryTag(src.primaryTag),
        subTag(src.subTag),
        qValue(src.qValue)
{}


const SipAcceptLanguage&
SipAcceptLanguage::operator =(const SipAcceptLanguage& src)
{
    if (&src != this)
    {
        primaryTag = src.primaryTag;
        subTag = src.subTag;
        qValue = src.qValue;
    }
    return (*this);
}

bool SipAcceptLanguage::operator ==( const SipAcceptLanguage& src) const
{
    return ( (qValue == src.qValue) &&
             (primaryTag == src.primaryTag) &&
             (subTag == src.subTag)
           );
 
}





void SipAcceptLanguage::setPrimaryTag(const Data& srcPrimaryTag)
{
    primaryTag = srcPrimaryTag;
}

Data SipAcceptLanguage::getPrimaryTag() const
{
    return primaryTag;
}

void SipAcceptLanguage::setSubTag(const Data& srcSubTag)
{
    subTag = srcSubTag;
}

Data SipAcceptLanguage::getSubTag() const
{
    return subTag;
}

void SipAcceptLanguage::setqValue(const Data& srcqValue)
{
    qValue = srcqValue;
}

Data SipAcceptLanguage::getqValue() const
{
    return qValue;
}


Data SipAcceptLanguage::encode() const
{
    Data data1;
    if (primaryTag.length())
    {
        data1 = primaryTag;
    }
    if (subTag.length())
    {
        data1 += "-";
        data1 += subTag;
    }
    if (qValue.length())
    {
        data1 += ";";
        data1 += "q=";
        data1 += qValue;
    }
    return data1;

}



SipHeader*
SipAcceptLanguage::duplicate() const
{
    return new SipAcceptLanguage(*this);
}


bool
SipAcceptLanguage::compareSipHeader(SipHeader* msg) const
{
    SipAcceptLanguage* otherMsg = dynamic_cast<SipAcceptLanguage*>(msg);
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
