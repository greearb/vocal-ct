#ifndef RtspSession_hxx
#define RtspSession_hxx
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


static const char* const RtspSession_hxx_Version =
    "$Id: RtspSession.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtpSession.hxx"
#include "RtspSessionStats.hxx"
#include "RtspFileHandler.hxx"
#include "State.hxx"

#include "RtspMsg.hxx"
#include "RtspSetupMsg.hxx"
#include "RtspPlayMsg.hxx"
#include "RtspPauseMsg.hxx"

#include "Sptr.hxx"
#include "Fifo.h"
#include "TimeVal.hxx"
#include "NetworkAddress.h"
#include "Connection.hxx"

#include <sys/time.h>
#include <unistd.h>


/** session mode enum */
enum RtspSessionMode
{
    RTSP_SESSION_MODE_PLAY,
    RTSP_SESSION_MODE_REC,
    RTSP_SESSION_MODE_UNKNOWN
};

 
/** Class to keep per session parameters.
    <ul>
    <li>sesion id
    <li>session state (init, playing, pause, etc)
    <li>session mode (play, rec, unknown>
    <li>client/server ip address
    <li>file handler and information
    <li>rtp stack and port information
    <li>pause points
    <li>etc
    </ul>
 */
class RtspSession 
{
    public:
        friend class RtspRtpProcessor;

        /** constructor */
        RtspSession();
        /** deconstructor */
        virtual ~RtspSession();

        /** adds an event into pending play events fifo */
        void addPendingEvent( Sptr<RtspPlayMsg> event );

        /** set session state */
        void state( Sptr<State> state )
            { myState = state; }
        /** get session state */
        Sptr<State> state() const
            { return myState; }

        /** set sessionId */
        void sessionId( const Data& sessionId )
            { mySessionId = sessionId; }
        /** get sessionId */
        Data sessionId() const
            { return mySessionId; }

        /** set rtsp transport spec - from SETUP method */
        void transport( const RtspTransportSpec& spec )
            { myTransportSpec = spec; }
        /** get rtsp transport spec - from SETUP method */
        const RtspTransportSpec& transport() const
            { return myTransportSpec; }

        /** set server network address */
        void clientAddress( const NetworkAddress& addr )
            { myClientAddr = addr; }
        /** get server network addrees */
        const NetworkAddress& clientAddress() const
            { return myClientAddr; }

        /** set client network address */
        void serverAddress( const NetworkAddress& addr )
            { myServerAddr = addr; }
        /** get client network address */
        const NetworkAddress& serverAddress() const
            { return myServerAddr; }

        /** set file handler */
        void fileHandler( Sptr<RtspFileHandler> fh )
            { myFileHandler = fh; }
        /** get file handler */
        Sptr<RtspFileHandler> fileHandler() const
            { return myFileHandler; }

        /** Create rtp stack based on current transport values.
            Must be called after opening file handler.
            @param mode play = 0, record = 1
            @return false on failure
         */
        bool createRtpStack( const int mode );
        /** delete rtp stack */
        void deleteRtpStack();

        /** get rtp session pointer */
        RtpSession* rtpSession() const
            { return myRtpSession; }

        /** get session mode */
        const RtspSessionMode sessionMode() const
            { return mySessionMode; }

        /** set rtp fd - used for select() loop */
        void rtpFd( const int fd )
            { myRtpFd = fd; }
        /** get rtp fd - used for select() loop */
        int rtpFd() const
            { return myRtpFd; }

        /** save pending pause msg */
        void pendingPause( Sptr<RtspPauseMsg> msg )
            { myPauseMsg = msg; }
        /** get pending pause msg */
        Sptr<RtspPauseMsg> pendingPause() const
            { return myPauseMsg; }

        /** save orginal setup msg - not really used by anyone */
        void setupMsg( Sptr<RtspSetupMsg> msg )
            { mySetupMsg = msg; }
        /** save orginal setup msg - not really used by anyone */
        Sptr<RtspSetupMsg> setupMsg() const
            { return mySetupMsg; }


        /** inital fifo time when session was added to rtp processor */
        TimeVal myFifoTimeBase;
        /** current play position in ms */
        long myCurrentNpt;
        /** pending pause point in npt */
        long myPausePoint;

        /** total number of packets to play, -1 if unknown */
        int myPacketTotal;
        /** number of packets currently played */ 
        int myPacketCounter;
        /** packet time interval in ms */
        int myPacketIntervalMs;

        /** per sessoin statistics */
        RtspSessionStats myStats;
        /** lock for updating statistics */
        VMutex myStatsMutex;
        /** */
        PerRecordReqData myPerRecordReqData;
        /** */
        PerPlayReqData myPerPlayReqData;

    private:
        /** print rtp port information - used for debugging */
        void printRtpInfo();

    private:
        /** session Id */
        Data mySessionId;

        /** current state */
        Sptr<State> myState;

        /** client rtsp connection information */
        NetworkAddress myClientAddr;
        /** server rtsp connection information */
        NetworkAddress myServerAddr;

        /** rtp connection parameters */
        RtspTransportSpec myTransportSpec;

        /** file to be played or recorded */
        Sptr<RtspFileHandler> myFileHandler;

        /** rtp stack pointer */
        RtpSession* myRtpSession;

        /** session mode, play, record, or unknown */
        RtspSessionMode mySessionMode;

        /** rtp fd - used for select() loops */
        int myRtpFd;

        /** fifo of pending play requests */
        Fifo< Sptr<RtspPlayMsg> > myPendingEvents;

        /** pending play request*/
        Sptr<RtspPauseMsg> myPauseMsg;

        /** orginal setup request */
        Sptr<RtspSetupMsg> mySetupMsg;

    private:
        /** suppress copy constructor */
        RtspSession( const RtspSession& );
        /** suppress assignment operator */
        RtspSession& operator=( const RtspSession& );
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
