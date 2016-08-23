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
#include "SipSubsNotifyEvent.hxx"
#include "symbols.hxx"
#include "SipParserMode.hxx"
#include "cpLog.h"

using namespace Vocal;


string
SipSubsNotifyEventParserException::getName( void ) const
{
    return "SipSubsNotfiyEventParserException";
}

SipSubsNotifyEvent::SipSubsNotifyEvent( const Data& srcData, const string& local_ip )
    : SipHeader(local_ip)
{
    if (srcData == "") {
        return;
    }
    try
    {
        decode(srcData);
        eventType = srcData;
    }
    catch (SipSubsNotifyEventParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Constructor :o( ");
            throw SipSubsNotifyEventParserException("failed in Decode",
                                                    __FILE__,
                                                    __LINE__,
                                                    DECODE_EVENT_FAILED);
        }
    }
}


SipSubsNotifyEvent::SipSubsNotifyEvent( const SipSubsNotifyEvent& src)
    : SipHeader(src)
{
    eventType = src.getEventType();
}

bool
SipSubsNotifyEvent::operator==(const SipSubsNotifyEvent& src) const
{
    cpLog(LOG_DEBUG_STACK, "SipSubsNotifyEvent ::operator ==");

    bool equal = false;
    cpLog(LOG_DEBUG_STACK, "this.event type = %s", eventType.logData());
    cpLog(LOG_DEBUG_STACK, "other.event type = %s", src.getEventType().logData());
    equal = (eventType == src.getEventType());

    cpLog(LOG_DEBUG_STACK, "SubsNotifyEvent operator == final result: %d", equal);

    return equal;
}


SipSubsNotifyEvent &
SipSubsNotifyEvent::operator=(const SipSubsNotifyEvent& src)
{
    if (&src != this)
    {
        eventType = src.eventType;
    }
    return (*this);
}


Data
SipSubsNotifyEvent::getEventType() const
{
    return eventType;
}


void
SipSubsNotifyEvent::setEventType( const Data& item)
{
    eventType = item;
}


Data
SipSubsNotifyEvent::encode() const
{
    Data sipSubsNotifyEvent = EVENT;

    sipSubsNotifyEvent += SP;
    sipSubsNotifyEvent += eventType;

    sipSubsNotifyEvent += CRLF;

    return sipSubsNotifyEvent;
}


void
SipSubsNotifyEvent::decode( const Data& data )
{
    try
    {
        parse(data);
    }
    catch (SipSubsNotifyEventParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in decode() :o( ");

            throw SipSubsNotifyEventParserException("failed in Decode",
                                                    __FILE__,
                                                    __LINE__,
                                                    DECODE_EVENT_FAILED);
        }
    }
}


void
SipSubsNotifyEvent::parse( const Data& data)
{
    try
    {
        scanSipSubsNotifyEvent(data);
    }
    catch (SipSubsNotifyEventParserException&)
    {
        if (SipParserMode::sipParserMode())
        {
            cpLog(LOG_ERR, "Failed to Decode in Parse :o( ");

            throw SipSubsNotifyEventParserException("failed in Decode",
                                                    __FILE__,
                                                    __LINE__,
                                                    DECODE_EVENT_FAILED);
        }

    }
    //No parse problems.
}


void
SipSubsNotifyEvent::scanSipSubsNotifyEvent(const Data& data)
{
    eventType = data;
}


Sptr<SipHeader>
SipSubsNotifyEvent::duplicate() const
{
    return new SipSubsNotifyEvent(*this);
}


bool
SipSubsNotifyEvent::compareSipHeader(SipHeader* msg) const
{
    SipSubsNotifyEvent* otherMsg = dynamic_cast<SipSubsNotifyEvent*>(msg);
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
