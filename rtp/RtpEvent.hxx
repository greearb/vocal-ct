#ifndef RTPEVENT_HXX
#define RTPEVENT_HXX

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

static const char* const RtpEvent_hxx_Version =
    "$Id: RtpEvent.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtpPacket.hxx"
#include "DTMFInterface.hxx"

/**
   Class which contains an RTP event, such as a DTMF (touch tone) message.
   Many devices transmit DTMF via non-audible signals instead of actually
   sending the audio data, and the RtpEvent class is used in this context.

   @see RtpSession
*/
class RtpEvent
{
    public:
        ///
        RtpEvent();
        ///
        virtual ~RtpEvent();


        /// send out a DTMFEvent in RTP
        void sendEvent( const DTMFEvent event );

        /// receives DTMFEvent from RTP
        /// calls the registered callback fucntion
        void recvEvent( RtpPacket* p );

        ///
        const DTMFInterface* const getDTMFInterface()
        {
            return _DTMFInterface;
        }

        /// set DTMF callback function
        void setDTMFInterface( DTMFInterface* t )
        {
            _DTMFInterface = t;
        }

        /// unset DTMF callback function
        void unsetDTMFInterface( DTMFInterface* t )
        {
            _DTMFInterface = 0;
        }


        /// changes type of DTMF in rtp to send
        void setDTMFType( const RtpPayloadType DTMFType );


        /// (what does this do?)
        void eventDTMF( int a, int b )
        {
            _DTMFInterface->sendDTMF( a, b );
        }

    private:
        // sending events
        void sendEventRFC2833( const DTMFEvent event );
        void sendEventCiscoRTP( const DTMFEvent event );

        // reading events
        void recvEventRFC2833( RtpPacket* p );
        void recvEventCiscoRTP( RtpPacket* p );
        void sendToDTMFInterface( const DTMFEvent event );

        DTMFEvent DTMFToEvent( int event );

        // device interface
        DTMFInterface* _DTMFInterface;

        // state information
        RtpPayloadType _DTMFType;
        KeyEvent _keyEvent;
        DTMFEvent _DTMFEvent;
};

#endif
