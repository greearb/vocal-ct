
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

static const char* const SipAcceptEncoding_cxx_Version =
    "$Id: SipAcceptEncoding.cxx,v 1.3 2004/11/19 01:54:38 greear Exp $";


#include "global.h"
#include "SipAcceptEncoding.hxx"
#include "SipParserMode.hxx"

using namespace Vocal;


string
SipAcceptEncodingParserException::getName( void ) const
{
    return "SipAcceptEncodingParserException";
}


///
SipAcceptEncoding::SipAcceptEncoding( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip)
{
    if (srcData != "") {
        try
        {
            decode(srcData);
        }
        catch (SipAcceptEncodingParserException&)
        {
            if (SipParserMode::sipParserMode())
            {
                
                throw SipAcceptEncodingParserException(
                    "failed to decode the AcceptEncoding string :(",
                    __FILE__,
                    __LINE__, DECODE_ACCEPTENCODING_FAILED);
            }
            
        }
    }
}

void SipAcceptEncoding::decode(const Data& vdata)
{
    try
    {

        scanAcceptEncoding(vdata);
    }
    catch (SipAcceptEncodingParserException&)
    {

        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptEncodingParserException(
                "failed to decode the AcceptEncoding string :(",
                __FILE__,
                __LINE__, DECODE_ACCEPTENCODING_FAILED);
        }


    }

}

void
SipAcceptEncoding::scanAcceptEncoding(const Data & scandata)
{
    Data aedata = scandata;
    Data aevalue;
    int ret = aedata.match(SEMICOLON, &aevalue, true);
    if (ret == FOUND)
    {
        
        Data qdata = aedata;
        Data qval;
        int retn = qdata.match(EQUAL, &qval, true);
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

                    throw SipAcceptEncodingParserException(
                        "failed to decode the AcceptEncoding string :(",
                        __FILE__,
                        __LINE__, DECODE_ACCEPTENCODING_FAILED);
                }

            }
        }
        else if (retn == NOT_FOUND)
        {
            if (SipParserMode::sipParserMode())
            {

                throw SipAcceptEncodingParserException(
                    "failed to decode the AcceptEncoding string :(",
                    __FILE__,
                    __LINE__, DECODE_ACCEPTENCODING_FAILED);
            }

        }
        else if (retn == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {

                throw SipAcceptEncodingParserException(
                    "failed to decode the AcceptEncoding string :(",
                    __FILE__,
                    __LINE__, DECODE_ACCEPTENCODING_FAILED);
            }

        }
    }

    else if (ret == NOT_FOUND)
    {
      //do nothing
    }
    else if (ret == FIRST)
    {

        if (SipParserMode::sipParserMode())
        {

            throw SipAcceptEncodingParserException(
                "failed to decode the AcceptEncoding string :(",
                __FILE__,
                __LINE__, DECODE_ACCEPTENCODING_FAILED);
        }

    }

}
///
SipAcceptEncoding::SipAcceptEncoding( const SipAcceptEncoding& src)
    : SipHeader(src),
        contentCoding(src.contentCoding),
        qValue(src.qValue)
{}


const SipAcceptEncoding&
SipAcceptEncoding::operator =(const SipAcceptEncoding& src)
{
    if (&src != this)
    {
        contentCoding = src.contentCoding;
        qValue = src.qValue;
    }
    return (*this);
}

bool SipAcceptEncoding::operator ==( const SipAcceptEncoding& src) const
{
    return ( (contentCoding == src.contentCoding) &&
             (qValue == src.qValue)
           );
}


Data SipAcceptEncoding::getContentCoding() const
{
    return contentCoding;
}

void SipAcceptEncoding::setContentCoding(const Data& srcContentCoding)
{
    contentCoding = srcContentCoding;
}

Data SipAcceptEncoding::getqValue() const
{
    return qValue;
}

void SipAcceptEncoding::setqValue(const Data& srcqValue)
{
    qValue = srcqValue;
}



///
Data SipAcceptEncoding::encode() const
{
    Data data2;
    if (contentCoding.length())
    {
        data2 = contentCoding;
        if (qValue.length())
        {
            data2 += SEMICOLON;
            data2 += "q=";
            data2 += qValue;
        }
    }
    return data2;

}




Sptr<SipHeader>
SipAcceptEncoding::duplicate() const
{
    return new SipAcceptEncoding(*this);
}


bool
SipAcceptEncoding::compareSipHeader(SipHeader* msg) const
{
    SipAcceptEncoding* otherMsg = dynamic_cast<SipAcceptEncoding*>(msg);
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
