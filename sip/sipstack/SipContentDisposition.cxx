
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

static const char* const SipContentDisposition_cxx_Version =
    "$Id: SipContentDisposition.cxx,v 1.3 2004/11/19 01:54:38 greear Exp $";

#include "global.h"
#include "SipContentDisposition.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"
#include "symbols.hxx"

static const char SIP_CONTDISPOSITION[] = "Content-Disposition";


using namespace Vocal;


string
SipContentDispositionParserException::getName( void ) const
{
    return "SipContentDispositionParserException";
}

SipContentDisposition::SipContentDisposition(const SipContentDisposition & src)
        : SipHeader(src),
          attribute(src.attribute),
        value(src.value),
        handleparm(src.handleparm),
	dispositiontype(src.dispositiontype)
{
}


SipContentDisposition::SipContentDisposition( const Data& newData, const string& local_ip )
    : SipHeader(local_ip)
{
    if (newData == "") {
        return;
    }
    try
    {
        decode(newData);
    }
    catch (SipContentDispositionParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentDispositionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTDISPOSITION_FAILED);

        }

    }

}
void SipContentDisposition::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipContentDisposition(nData);
    }
    catch (SipContentDispositionParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentDispositionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTDISPOSITION_FAILED);

        }

    }

}

void
SipContentDisposition::scanSipContentDisposition(const Data &tmpdata)
{
    Data despdata = tmpdata;
    Data despvalue;
    int test = despdata.match(";", &despvalue, true);
    if (test == FOUND)
    {
        setDispositionType(despvalue);
        parseDispositionParm(despdata);
    }
    else if (test == NOT_FOUND)
    {
        setDispositionType(despdata);
    }
    else if (test == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipContentDispositionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTDISPOSITION_FAILED);

        }
    }

}

void
SipContentDisposition::parseDispositionParm(const Data &tmepdata)
{

    Data parmdata = tmepdata;
    Data parmvalue;
    while (parmdata.length())
    {
        int test = parmdata.match(";", &parmvalue, true);
        if (test == FOUND)
        {
            parseParms(parmvalue);

        }
        else if (test == NOT_FOUND)
        {
            parseParms(parmdata);
            break;
        }
        else if (test == FIRST)
        {}

    }

}
void
SipContentDisposition::parseParms(const Data &tpdata)
{
    Data dsdata = tpdata;
    Data dsvalue;
    int test = dsdata.match("=", &dsvalue, true);
    if (test == FOUND)
    {
        parseFinParms(dsvalue, dsdata);
    }
    else if (test == NOT_FOUND)
    {

    }

    else if (test == FIRST)
    {
    }

}

SipContentDisposition& SipContentDisposition::operator= ( const SipContentDisposition & src)
{
    if ( &src != this )
    {
        attribute = src.attribute;
        value = src.value;
        handleparm = src.handleparm;
        dispositiontype = src.dispositiontype;
    }
    return (*this);
}

bool SipContentDisposition::operator== ( const SipContentDisposition & src) const
{
    return (
       ( attribute == src.attribute ) && 
       ( value == src.value ) && 
       ( handleparm == src.handleparm ) &&
       ( dispositiontype == src.dispositiontype )    );
}


void
SipContentDisposition::parseFinParms(const Data &tpdata, const Data &tvalue)
{
    Data dspdata = tpdata;
    Data dspvalue = tvalue;
    if (dspdata == HANDLING_PARM)
    {
        if (dspvalue == CONT_OPTIONAL || dspvalue == CONT_REQUIRED)
        {
            setHandleParm(dspvalue);
        }
        else
        {
            if (SipParserMode::sipParserMode())
            {
                cpLog(LOG_ERR, "Failed to Decode in parseFinParms Handle Parm is not (optional | required) :o( ");
                throw SipContentDispositionParserException("failed in Decode", __FILE__, __LINE__, DECODE_CONTENTDISPOSITION_FAILED);

            }

        }
    }
    else

    {
        setAttribute(dspdata);
        setValue(dspvalue);
    }
}



Data SipContentDisposition::encode() const
{
    Data ret;
    Data dispType = getDispositionType();
    Data handleParm = getHandleParm();
    Data attr = getAttribute();
    Data val = getValue();

    if ( (dispType.length()) ||
	 (handleParm.length()) ||
	 (attr.length())
       )
    {
        ret += SIP_CONTDISPOSITION ;
        ret += ": ";
        ret += dispType;
        if (handleParm.length())
	{
            ret += ";";
            ret += HANDLING_PARM;
            ret += "=";
            ret += handleParm;
	}
        if ( (attr.length()) &&
             (val.length())
            )
        {
            ret += ";";
            ret += attr;
            ret += "=";
            ret += val;
        }
        ret += CRLF;
    }
    return ret;

}


Sptr<SipHeader>
SipContentDisposition::duplicate() const
{
    return new SipContentDisposition(*this);
}


bool
SipContentDisposition::compareSipHeader(SipHeader* msg) const
{
    SipContentDisposition* otherMsg = dynamic_cast<SipContentDisposition*>(msg);
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
