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
#include "Rtp.hxx"
#include "RtpEventReceiver.hxx"
#include "RtpPacket.hxx"
#include "cpLog.h"


RtpEventReceiver::RtpEventReceiver()
{
    _DTMFInterface = 0;
    _keyEvent = KeyEventNULL;
    _DTMFEvent = DTMFEventNULL;
    return;
}


RtpEventReceiver::~RtpEventReceiver()

{
    return;
}


void RtpEventReceiver::recvEvent( RtpPacket& pkt )
{
    if( _DTMFInterface == 0 )
        return;

    switch( pkt.getPayloadType() )
    {
        case( rtpPayloadDTMF_RFC2833 ):
            recvEventRFC2833( pkt );
            break;
        case( rtpPayloadCiscoRtp ):
            recvEventCiscoRTP( pkt );
            break;
        default:
            cpLog(LOG_ERR, "Unknown rtp payload event");
            return;
    }

    return;
}


void RtpEventReceiver::recvEventRFC2833( RtpPacket& pkt )
{
    assert(pkt.getPayloadType() == rtpPayloadDTMF_RFC2833);

    assert(pkt.getPayloadUsage() != 0);
    assert(pkt.getHeader() != 0);

    RtpEventDTMFRFC2833* e = reinterpret_cast<RtpEventDTMFRFC2833*> (pkt.getPayloadLoc());
    assert(e != NULL);

    if( e->edge == 1)
    {
        if( _keyEvent == KeyEventOn ) {
            // off detection
            cpLog(LOG_DEBUG_STACK, "DTMF off detected: %d", _DTMFEvent);
            sendToDTMFInterface ( _DTMFEvent, e->duration );
            _keyEvent = KeyEventNULL;
        }
    }
    else {
       if (_keyEvent == KeyEventNULL) {
          // on detection
          _keyEvent = KeyEventOn;
          _DTMFEvent = DTMFToEvent( e->event );
          cpLog(LOG_DEBUG_STACK, "DTMF on detected: %d", _DTMFEvent);
       }
    }
}

void RtpEventReceiver::recvEventCiscoRTP( RtpPacket& pkt )
{
    assert(pkt.getPayloadType() == rtpPayloadCiscoRtp);

    RtpEventDTMFCiscoRtp* e = reinterpret_cast<RtpEventDTMFCiscoRtp*> (pkt.getPayloadLoc());

    if ( (_keyEvent != KeyEventOn) && (e->digitType == 1) )
    {
        // on detection
        _keyEvent = KeyEventOn;
        _DTMFEvent = DTMFToEvent(e->digitCode);
        cpLog(LOG_DEBUG_STACK, "DTMF on detected: %d", _DTMFEvent);
    }
    else if ( (_keyEvent == KeyEventOn) && (e->digitType == 0) && (e->edge == 0) )
    {
        // off detection
        cpLog(LOG_DEBUG_STACK, "DTMF off detected: %d", _DTMFEvent);
        sendToDTMFInterface( _DTMFEvent, 20 );
        _keyEvent = KeyEventNULL;
    }
}


void RtpEventReceiver::sendToDTMFInterface( const DTMFEvent e, 
					    unsigned int duration )
{
    if( _DTMFInterface != 0 )
    {
        cpLog(LOG_DEBUG_STACK, "Sending event %d to DTMFInterface", e);
        _DTMFInterface->sendDTMF( e, duration );
    }
    return ;
}


DTMFEvent RtpEventReceiver::DTMFToEvent( int event )
{
    DTMFEvent result;

    switch( event )
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
