#ifndef RtspServer_hxx
#define RtspServer_hxx
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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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


static const char* const RtspServer_hxx_Version =
    "$Id: RtspServer.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspMsgInterface.hxx"
#include "RtspSession.hxx"
#include "StateMachine.hxx"

#include "Sptr.hxx"
#include "NetworkAddress.h"
#include "RtspRequest.hxx"
#include "RtspServerStats.hxx"


/** Process the message from the RtspMsgInterface in a seperate thread.  
    <ul>
    <li>Non-session requests (ANNOUNCE, DESCRIBE, SETUP, etc) are handle in 
        the one state StateMachine.
    <li>Session requests (PLAY, RECORD, PAUSE, TEARDOWN) are process through
        the state machine.
    <li>Server requests (sessionId = "Global") are processed directly.
    </ul>
 */
class RtspServer
{
    public:
        /** Create a singelton object.  Creates the state machine; read the
            configuration file, build the rtp processor, and starts all the
            threads */
        static RtspServer& instance( const int port = 544 );
        /** destructor */
        virtual ~RtspServer();
        /**  frees the memory associated with singelton instance */
        static void destroy( void );

        /** Process the rtsp messages forever */
        void run();
        /** Stops rtspServer thread.  Teardowns all the active rtsp sessions
            @param delay delay time until shutdown (not implemented yet)
        */
        void stop( const int delay = 0 );

        /** sends rtsp response - interface between RtspMsgInterface and
            StateMachine */
        void sendResponse( Sptr<RtspResponse> response );

        /** set server network address */
        void networkAddress( const NetworkAddress& addr )
            { myNetworkAddress = addr; }
        /** get server network address */
        const NetworkAddress& networkAddress()
            { return myNetworkAddress; }

        /** server statistics */
        RtspServerStats myStats;
        /** lock around server statistics */
        VMutex myStatsMutex;

    protected:
        /** processes requests with no sessionId */
        bool processRequest( Sptr<RtspRequest> msg );

        /** processes requests with a sessionId */
        bool processSessionRequest( Sptr<RtspRequest> msg );

        /** processes requests with a sessionId = "Global" */
        bool processServerRequest( Sptr<RtspRequest> msg );

    private:
        /** constructor */
        RtspServer( const int port );

        /** singelton object */
        static RtspServer* myInstance;

        /** interface into rtsp messages */
        RtspMsgInterface myMsgInterface;

        /** rtsp server network address */
        NetworkAddress myNetworkAddress;

    protected:
        /** suppress copy constructor */
        RtspServer( const RtspServer& );
        /** suppress assignment operator */
        RtspServer& operator=( const RtspServer& );
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
