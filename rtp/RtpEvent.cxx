
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

static const char* const RtpEvent_cxx_Version =
    "$Id: RtpEvent.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include "global.h"
#include "cpLog.h"
#include "Rtp.hxx"
#include "RtpEvent.hxx"
#include "RtpPacket.hxx"


RtpEvent::RtpEvent()
{
    _DTMFInterface = 0;
    _DTMFType = rtpPayloadDTMF_RFC2833;
    _keyEvent = KeyEventNULL;
    _DTMFEvent = DTMFEventNULL;
    return ;
}


RtpEvent::~RtpEvent()
{
    return ;
}


void RtpEvent::setDTMFType( const RtpPayloadType DTMFType )
{
    switch( DTMFType )
    {
        case( rtpPayloadDTMF_RFC2833 ):
        case( rtpPayloadCiscoRtp ):
            _DTMFType = DTMFType;
        default:
            cpLog(LOG_ERR, "Can't set RtpEvent to type %d", DTMFType );
            return;
    }

    return;
}


void RtpEvent::sendEvent( const DTMFEvent event )
{
    switch( _DTMFType )
    {
        case( rtpPayloadDTMF_RFC2833 ):
            sendEventRFC2833( event );
            break;
        case( rtpPayloadCiscoRtp ):
            //sendEventCiscoRTP( event );
            break;
        default:
            cpLog(LOG_ERR, "Unknown rtp payload event");
            return ;
    }

    return;
}


void RtpEvent::sendEventRFC2833( const DTMFEvent event )
{

    return;
}


void RtpEvent::recvEvent( RtpPacket* p )
{
    if ( _DTMFInterface == 0 )
        return ;

    switch( p->getPayloadType() )
    {
        case( rtpPayloadDTMF_RFC2833 ):
            recvEventRFC2833( p );
            break;
        case( rtpPayloadCiscoRtp ):
            recvEventCiscoRTP( p );
            break;
        default:
            cpLog(LOG_ERR, "Unknown rtp payload event");
            return ;
    }

    return ;
}


void RtpEvent::recvEventRFC2833( RtpPacket* p )
{
    assert(p->getPayloadType() == rtpPayloadDTMF_RFC2833);

    RtpEventDTMFRFC2833* e = NULL;

    if ( p->getPayloadUsage() != 0 )
        e = reinterpret_cast < RtpEventDTMFRFC2833* > (p->getPayloadLoc());

    if ( e == NULL )
    {
        if ( _keyEvent == KeyEventOn )
        {
            // off detection
            cpLog(LOG_DEBUG_STACK, "DTMF off detected: %d", _DTMFEvent);
            sendToDTMFInterface ( _DTMFEvent );
            _keyEvent = KeyEventNULL;
        }
    }
    else if ( _keyEvent != KeyEventOn && !e->edge )
    {
        // on detection
        _keyEvent = KeyEventOn;
        _DTMFEvent = DTMFToEvent( e->event );
        cpLog(LOG_DEBUG_STACK, "DTMF on detected: %d", _DTMFEvent);
    }

    return ;
}

void RtpEvent::recvEventCiscoRTP( RtpPacket* p )
{
    assert(p->getPayloadType() == rtpPayloadCiscoRtp);

    RtpEventDTMFCiscoRtp* e = reinterpret_cast < RtpEventDTMFCiscoRtp* > (p->getPayloadLoc());

    if ( _keyEvent != KeyEventOn && e->digitType == 1 )
    {
        // on detection
        _keyEvent = KeyEventOn;
        _DTMFEvent = DTMFToEvent(e->digitCode);
        cpLog(LOG_DEBUG_STACK, "DTMF on detected: %d", _DTMFEvent);
    }
    else if ( _keyEvent == KeyEventOn && e->digitType == 0 && e->edge == 0 )
    {
        // off detection
        cpLog(LOG_DEBUG_STACK, "DTMF off detected: %d", _DTMFEvent);
        sendToDTMFInterface( _DTMFEvent );
        _keyEvent = KeyEventNULL;
    }

    return ;
}


void RtpEvent::sendToDTMFInterface( const DTMFEvent e )
{
    cpLog(LOG_DEBUG_STACK, "Adding event %d to fifo", e);
    if ( _DTMFInterface != 0 )
        _DTMFInterface->sendDTMF( e, 20 );
    return ;
}


DTMFEvent RtpEvent::DTMFToEvent( int event )
{
    DTMFEvent result;

    switch ( event )
    {
        case 0:
        result = DTMFEventDigit0;
        break;
        case 1:
        result = DTMFEventDigit1;
        break;
        case 2:
        result = DTMFEventDigit2;
        break;
        case 3:
        result = DTMFEventDigit3;
        break;
        case 4:
        result = DTMFEventDigit4;
        break;
        case 5:
        result = DTMFEventDigit5;
        break;
        case 6:
        result = DTMFEventDigit6;
        break;
        case 7:
        result = DTMFEventDigit7;
        break;
        case 8:
        result = DTMFEventDigit8;
        break;
        case 9:
        result = DTMFEventDigit9;
        break;
        case 10:
        result = DTMFEventDigitStar;
        break;
        case 11:
        result = DTMFEventDigitHash;
        break;
        default:
        cpLog(LOG_ERR, "Unknown DTMF event during conversion");
        result = DTMFEventNULL;
    }
    return result;
}
