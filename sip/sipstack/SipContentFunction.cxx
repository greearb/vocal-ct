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

static const char* const SipContentFunction_cxx_Version =
    "$Id: SipContentFunction.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipContentFunction.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

static const char SIP_CONTFUNCTION[] = "Content-Encoding ";




string
SipContentFunctionParserException::getName( void ) const
{
    return "SipContentFunctionParserException";
}

SipContentFunction::SipContentFunction(SipContentFunction & src)
        : data(src.data)
{}


SipContentFunction::SipContentFunction(void)
{
}


SipContentFunction& SipContentFunction::operator=( const SipContentFunction& src)
{
    if ( &src != this)
    {
        data = src.data;
    }
    return (*this);
}



SipContentFunction::SipContentFunction( const Data& newData )
{
    try
    {
        decode(newData);
    }
    catch (SipContentFunctionParserException& expection)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentFunctionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTFUNCTION_FAILED);

        }

    }

}
void SipContentFunction::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipContentFunction(nData);
    }
    catch (SipContentFunctionParserException exception)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentFunctionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTFUNCTION_FAILED);

        }

    }

}

void
SipContentFunction::scanSipContentFunction(const Data &tmpdata)
{
    string newdata = tmpdata ;

    set(newdata);
}


Data SipContentFunction::get()
{
    return data;
}

void SipContentFunction::set( const Data& newData )
{
    data = newData;

}

Data SipContentFunction::encode()
{
    Data ret;
    // if (data.length())
    ret += SIP_CONTFUNCTION ;
    ret += ":";
    ret += data ;

    return ret;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
