
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

static const char* const SipRetryAfter_cxx_Version =
    "$Id: SipRetryAfter.cxx,v 1.2 2004/06/16 06:51:25 greear Exp $";

#include "global.h"
#include "SipRetryAfter.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipRetryAfterParserException::getName( void ) const
{
    return "SipRetryAfterParserException";
}

///

SipRetryAfter::SipRetryAfter( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip),
      date("", local_ip),
      flagcomment(false),
      flagduration(false)
{

    if (srcData == "") {
        return;
    }

    try
    {
        decode(srcData);
    }
    catch (SipRetryAfterParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                "failed to decode the Retry After string",
                __FILE__,
                __LINE__, DECODE_RETRYAFTER_FAILED);
        }
    }

}


SipRetryAfter&
SipRetryAfter::operator=(const SipRetryAfter& rhs)
{
    if ( &rhs != this )
    {
        date = rhs.date;
        datedata = rhs.datedata;
        comment = rhs.comment;
        flagcomment = rhs.flagcomment;
        flagduration = rhs.flagduration;
    }
    return *this;
}


void SipRetryAfter::decode(const Data& retrydata)
{

    Data data = retrydata;

    try
    {

        scanRetryAfter(data);
    }
    catch (SipRetryAfterParserException& exception)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                exception.getDescription(),
                __FILE__,
                __LINE__, SCAN_RETRYAFTER_FAILED);

        }
    }

}
void
SipRetryAfter::scanRetryAfter(const Data &tmpdata)
{

    Data rtdata = tmpdata;
    Data newvalue;
    int retn = rtdata.match("(", &newvalue, true);
    if (retn == NOT_FOUND)
    {
        Data rtvalue;
        int ret = rtdata.match(";", &rtvalue, true);
        if (ret == FOUND)
        {
            parseDate(rtvalue);
            parseDuration(rtdata);
        }
        else if (ret == NOT_FOUND)
        {
            parseDate(rtdata);

        }
        else if (ret == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipRetryAfterParserException(
                    "failed to decode the Retry After string in scanRetryAfter :(",
                    __FILE__,
                    __LINE__, DECODE_RETRYAFTER_FAILED);
            }
        }

    }
    else if (retn == FOUND)
    {

        parseDate(newvalue);
        Data finald = rtdata;
        Data finalv;
        int rt = finald.match(")", &finalv, true);
        if (rt == FOUND)
        {
            setComment(finalv);
            Data valuei;
            int reth = finald.match(";", &valuei, true);
            if (reth == FOUND)
            {
                if (SipParserMode::sipParserMode())
                {
                    throw SipRetryAfterParserException(
                        "failed to decode the Retry After string in scanRetryAfter :(",
                        __FILE__,
                        __LINE__, DECODE_RETRYAFTER_FAILED);
                }
            }
            else if (reth == NOT_FOUND)
            {
            }

            else if (reth == FIRST)
            {
                parseDuration(finald);
            }
        }
        else if (rt == NOT_FOUND)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipRetryAfterParserException(
                    "failed to decode the Retry After string in  scanRetryAfter :(",
                    __FILE__,
                    __LINE__, DECODE_RETRYAFTER_FAILED);
            }
        }
        else if (rt == FIRST)
        {
            if (SipParserMode::sipParserMode())
            {
                throw SipRetryAfterParserException(
                    "failed to decode the Retry After string in  scanRetryAfter :(",
                    __FILE__,
                    __LINE__, DECODE_RETRYAFTER_FAILED);
            }

        }

    }
    else if (retn == FIRST)
    {

        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                "failed to decode the Retry After string in scanRetryAfter :(",
                __FILE__,
                __LINE__, DECODE_RETRYAFTER_FAILED);
        }
    }
}

void
SipRetryAfter::parseDuration(const Data& datdata)
{
    Data durdata = datdata;
    Data durvalue;
    int retn = durdata.match("=", &durvalue, true);
    if (retn == FOUND)
    {
        if (durdata == SIP_DURATION)
        {
            setDuration(durdata.convertInt());
        }
    }
    else if (retn == NOT_FOUND)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                "failed to decode the Retry After string  in ParseDuration :(",
                __FILE__,
                __LINE__, DECODE_RETRYAFTER_FAILED);
        }
    }
    else if (retn == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                "failed to decode the Retry After string in Parse Duration:(",
                __FILE__,
                __LINE__, DECODE_RETRYAFTER_FAILED);
        }
    }
}
void
SipRetryAfter::parseDate(const Data& dadata)
{
    Data dtdata = dadata;
    Data dtvalue;
    int retn = dtdata.match(",", &dtvalue, false);
    if (retn == FOUND)
    {
        SipDate datetmp(dtdata, getLocalIp());
        date = datetmp;
    }
    else if (retn == NOT_FOUND)
    {
        setDuration(dtdata.convertInt());
    }
    else if (retn == FIRST)
    {
        if (SipParserMode::sipParserMode())
        {
            throw SipRetryAfterParserException(
                "failed to decode the Retry After string in Parse Date :(",
                __FILE__,
                __LINE__, DECODE_RETRYAFTER_FAILED);
        }
    }

}

///
SipRetryAfter::SipRetryAfter( const SipRetryAfter& src )
    : SipHeader(src),
      date(src.date),
      datedata(src.datedata),
      comment(src.comment),
      duration(src.duration),
      flagcomment(src.flagcomment),
      flagduration(src.flagduration)
{}


bool
SipRetryAfter::operator ==(const SipRetryAfter& src) const
{
    cpLog(LOG_DEBUG_STACK, "Retry After == Operator ");

    if ( (date == src.date) &&
            (comment == src.comment) &&
            (duration == src.duration )
       )
    {
        cpLog(LOG_DEBUG_STACK, "Retry After == Operator  returning True");
        return true;
    }
    else
    {
        cpLog(LOG_DEBUG_STACK, "Retry After == Operator  returning False");
        return false;
    }

}


///
SipDate SipRetryAfter::getDate() const
{
    return date;
}

///
void SipRetryAfter::setDate( const SipDate& newdate )
{
    date = newdate;

}

///
Data SipRetryAfter::getComment() const
{
    return comment;
}

///
void SipRetryAfter::setComment( const Data& newcomment )
{
    comment = newcomment;
    flagcomment = true;
}

///
int SipRetryAfter::getDuration() const
{
    return duration.convertInt();

}

///
void
SipRetryAfter::setDuration( int newduration )
{
    duration = Data(newduration);
    flagduration = true ;
}


///
Data SipRetryAfter::encode() const
{
    Data ret;
    Data ldatedata = date.encode();

    if (ldatedata.length())
    {

        ret += SIP_RETRYAFTER;
        ret += SP;
        ret += ldatedata;
        if (flagcomment)
        {
            ret += comment;
        }
        if (flagduration)
        {
            ret += ";";
            ret += "duration";
            ret += "=";
            ret += duration;
        }
    }
    return ret;
}



// End of File


Sptr<SipHeader>
SipRetryAfter::duplicate() const
{
    return new SipRetryAfter(*this);
}


bool
SipRetryAfter::compareSipHeader(SipHeader* msg) const
{
    SipRetryAfter* otherMsg = dynamic_cast<SipRetryAfter*>(msg);
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
