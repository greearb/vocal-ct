
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

static const char* const SipTimestamp_cxx_Version =
    "$Id: SipTimestamp.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipTimestamp.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;

string
SipTimestampParserException::getName( void ) const
{
    return "SipTimeststampParserException";
}


SipTimestamp::SipTimestamp( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip)
{

    if (srcData == "") {
        return;
    }

    Data fdata = srcData;
    try
    {
        decode(fdata);
    }
    catch (SipTimestampParserException&)
    {

        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor of TimeStamp :( ");
            throw SipTimestampParserException(
                "failed to decode the Timestamp string :(",
                __FILE__,
                __LINE__, DECODE_TIMESTAMP_FAILED);
        }

    }

}
void SipTimestamp::decode(const Data& data)
{

    Data nData = data;

    try
    {

        scanSipTimestamp(nData);
    }
    catch (SipTimestampParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in TMIESTAMP decode():( ");
            throw SipTimestampParserException(
                "failed to decode the Timestamp string :(",
                __FILE__,
                __LINE__, DECODE_TIMESTAMP_FAILED);
        }
    }

}

void
SipTimestamp::scanSipTimestamp(const Data &tmpdata)
{
    setTime(tmpdata.convertInt());
}



SipTimestamp::SipTimestamp( const SipTimestamp& src )
    : SipHeader(src)
{
    time = src.time;
    delay = src.delay;
}

bool
SipTimestamp::operator ==(const SipTimestamp& src) const
{
    cpLog(LOG_DEBUG_STACK, "SipTimestamp, operator ==");
    if ( (time == src.time) &&
            (delay == src.delay)
       )
    {
        cpLog(LOG_DEBUG_STACK, "SipTimestamp,operator == returning true");
        return true;
    }
    else
    {

        cpLog(LOG_DEBUG_STACK, "SipTimestamp,operator == returning false");
        return false;
    }
}


int SipTimestamp::getTime()
{
    return time.convertInt();
}

void SipTimestamp::setTime( const int& newTime )
{
    time = Data(newTime);
}


int SipTimestamp::getDelay()
{
    return delay.convertInt();
}

void SipTimestamp::setDelay( const int& newDelay)
{
    delay = Data(newDelay);
}

Data SipTimestamp::getDelayData()
{
    return delay;
}

void SipTimestamp::setDelayData( const Data& newDelay )
{
    delay = newDelay;
}

Data SipTimestamp::encode() const
{
    Data data;
    if (time.length())
    {
        data += TIMESTAMP;
        data += SP;
        data += time;
        data += CRLF;
    }
    return data;
}


SipHeader*
SipTimestamp::duplicate() const
{
    return new SipTimestamp(*this);
}


bool
SipTimestamp::compareSipHeader(SipHeader* msg) const
{
    SipTimestamp* otherMsg = dynamic_cast<SipTimestamp*>(msg);
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
