#ifndef RtspRtpProcessor_hxx
#define RtspRtpProcessor_hxx
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

static const char* const RtspRtpProcessor_hxx_version =
    "$Id: RtspRtpProcessor.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include "RtspSession.hxx"

#include "Sptr.hxx"
#include <set>


/** Processes (play and record) rtp traffic for all the active sessions.<p>

    <b>Play</b> - Timing is controlled by a timer fifo.  When playing is done 
    (reach packet count or end of file reached), RtspRtpProcessor will move
    session into pause state.  Processor also enforces pause points and queued
    play requests for playing sessions.<p>

    <b>Recording</b> - Timing is controlled by a blocking select() on the 
    incoming rtp port.  Processor also enforces pause points.  However, queued
    record requests are not supported.<p>

    Play and record processing are done in seperate independent threads.
    Processor has overload dection mechanism to reject processing new
    sessions if cpu can't handle another session.<p>
 */
class RtspRtpProcessor
{
    public:
        /** create a singelton object */
        static RtspRtpProcessor& instance();
        /** destructor */
        virtual ~RtspRtpProcessor();
        /** frees the memory associated with singelton instance */
        static void destroy();


        /** start rtp processor threads */
        void startThread();
        /** stop rtp processor threads */
        void stopThread();


        /** build the rtp port pool. Ports are not really reserved in rtp stack
            @param low low rtp port range
            @param high high rtp port range
            @return false on falure (ie invalid port range). However, default 
                    values are still set.
         */
        bool buildRtpPortList( const int low, const int high );

        /** allocates a rtp port pair
            @return rtp and rtcp port(rtp+1) allocated, or 0 on no port are
                    available */
        int allocateRtpPort();

        /** deallocate a rtp port pair, false on failure
            @param port rtp port to release
            @return false of port not with high-low range (doesn't check if
                    port was really allocated
         */
        bool deallocateRtpPort( const int port );


        /** add inital rtp processor event.  Processor will place into play or
            record processor.
            @param session session to add to processor
            @return false on failure (ie invalid session state or processor
                    overloaded)
         */
        bool addInitalEvent( Sptr<RtspSession> session );
        /** delete session from rtp processor
            @return false if session in invalud state
         */
        bool delEvent( Sptr<RtspSession> session );

        /** check if play thread is loaded.  This check assumes play requests 
            are spread across time
         */
        bool playThreadLoaded();

        /** check if record thread is loaded.  This check assumes record 
            requests are spread across time
         */
        bool recordThreadLoaded();

    private:
        /** constructor */
        RtspRtpProcessor();

        /** singelton object */
        static RtspRtpProcessor* myInstance;

        // -- processor functions -----------------------
        /** set of free rtp ports */
        set<int> myRtpPortSet;
        /** inital low rtp port range */
        int myRtpPortLow;
        /** inital high rtp port range */
        int myRtpPortHigh;

        // -- play functions ----------------------------
        /** plays a rtp packet for a session */
        bool playRtpPacket( Sptr<RtspSession> );

        /** playing fifo - collection of playing sessions */
        Fifo< Sptr<RtspSession> > myPlayingFifo;

        /** common playing buffer for all sessions */
        char* myPlayBuffer;
        /** size of common playing buffer for all sessions */
        int myPlayBufferMax;

        // -- record functions --------------------------
        /** records a rtp packet for a session */
        void recordRtpPacket( Sptr<RtspSession> );
        /** RecordingList type */
        typedef list< Sptr<RtspSession> > RecordingList;
        /** recording list type - collection of recording session */
        RecordingList myRecordingList;
        /** recording list mutex */
        VMutex myRecordingListMutex;
        /** fd set mutex */
        VMutex myFdSetMutex;

        /** max recording select fd */
        int myMaxRecordFd;
        /** recording select fd */
        fd_set myBaseRecordFd;
        /** recording select load fd */
        fd_set myBaseRecordLoadFd;
        /** previous number of record flags set */
        int myPrevSelectResult;

        /** */
        RtpPacket* myRtpPacket;
        /** */
        char* myRecBuffer;
        /** */
        int myRecBufferMax;

        // -- thread functions --------------------------
        /** */
        static void* playThreadWrapper( void* p );
        /** */
        static void* recordThreadWrapper( void* p );
        /** */
        void* runPlayThread();
        /** */
        void* runRecordThread();
        /** thread for playing RTP traffic */
        VThread myPlayThread;
        /** thread for recording RTP traffic */
        VThread myRecordThread;
        /** */
        VMutex myShutdownMutex;
        /** */
        bool myShutdown;

    protected:
        /** suppress copy constructor */
        RtspRtpProcessor( const RtspRtpProcessor& );
        /** suppress assignment operator */
        RtspRtpProcessor& operator=( const RtspRtpProcessor& );
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
