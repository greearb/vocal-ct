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

#include "Rtp.hxx"
#include "RtpEventTransmitter.hxx"
#include "RtpPacket.hxx"


RtpEventTransmitter::RtpEventTransmitter( RtpTransmitter* rtpTran )
{
    _rtpTran = rtpTran;
    _DTMFType = rtpPayloadDTMF_RFC2833;
    _keyEvent = KeyEventNULL;
    _DTMFEvent = DTMFEventNULL;
    return ;
}


RtpEventTransmitter::~RtpEventTransmitter()
{
    return ;
}


void RtpEventTransmitter::setDTMFType( const RtpPayloadType DTMFType )
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


void RtpEventTransmitter::sendEvent( const DTMFEvent event )
{
    switch( _DTMFType )
    {
        case( rtpPayloadDTMF_RFC2833 ):
            sendEventRFC2833( event );
            break;
        case( rtpPayloadCiscoRtp ):
            sendEventCiscoRTP( event );
            break;
        default:
            cpLog(LOG_ERR, "Unknown rtp payload event");
            return ;
    }

    return;
}


void RtpEventTransmitter::sendEventRFC2833( const DTMFEvent event )
{
    

    return;
}



void RtpEventTransmitter::recvEventRFC2833( const DTMFEvent event )
{
    RtpEventDTMFRFC2833* e = NULL;

    if ( e->getPayloadUsage() != 0 )
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

void RtpEventTransmitter::recvEventCiscoRTP( const DTMFEvent event )
{
/*
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
*/
}
