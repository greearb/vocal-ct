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

static const char* const RtspRtpProcessor_cxx_version =
    "$Id: RtspRtpProcessor.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspServer.hxx"
#include "RtspRtpProcessor.hxx"
#include "StateMachine.hxx"
#include "OpPlay.hxx"
#include "OpPause.hxx"

#include <iostream>

static const int MIN_HANDLING_LOAD = 10;


// singelton object definition
RtspRtpProcessor* RtspRtpProcessor::myInstance = 0;

RtspRtpProcessor&
RtspRtpProcessor::instance()
{
    if( myInstance == 0 )
    {
        myInstance = new RtspRtpProcessor();
    }
    return *myInstance;
}


RtspRtpProcessor::RtspRtpProcessor()
    : myRtpPortLow( 8000 ),
      myRtpPortHigh( 8001 ),
      myPlayBuffer( 0 ),
      myPlayBufferMax( 0 ),
      myMaxRecordFd( 0 ),
      myPrevSelectResult( 0 ),
      myRecBuffer( 0 ),
      myRecBufferMax( 0 ),
      myShutdown( false )
{
#if defined (_linux_)    
    // register destroy function to delete singelton
    if( atexit( RtspRtpProcessor::destroy ) < 0 )
    {
        cpLog( LOG_ALERT, "Failed to register with atexit()" );
    }
#endif
}


RtspRtpProcessor::~RtspRtpProcessor()
{
}


void
RtspRtpProcessor::destroy()
{
    delete RtspRtpProcessor::myInstance;
    RtspRtpProcessor::myInstance = 0;
}


void*
RtspRtpProcessor::playThreadWrapper( void* p )
{
    return static_cast<RtspRtpProcessor*>(p)->runPlayThread();
}


void*
RtspRtpProcessor::recordThreadWrapper( void* p )
{
    return static_cast<RtspRtpProcessor*>(p)->runRecordThread();
}


void
RtspRtpProcessor::startThread()
{
    myPlayThread.spawn( recordThreadWrapper, this );
    myRecordThread.spawn( playThreadWrapper, this );
}


void
RtspRtpProcessor::stopThread()
{
    cpLog( LOG_DEBUG, "RtspRtpProcessor threads stopping" );

    // stop threads
    myShutdownMutex.lock();
    myShutdown = true;
    myShutdownMutex.unlock();

    // wait until threads complete
    myPlayingFifo.add( 0 );
    myPlayThread.join();
    myRecordThread.join();
}


bool
RtspRtpProcessor::addInitalEvent( Sptr<RtspSession> session )
{
    LocalScopeAllocator lo;

    if( session->sessionMode() == RTSP_SESSION_MODE_PLAY )
    {
        cpLog( LOG_DEBUG, "Adding session %s into playing fifo",
               session->sessionId().getData(lo) );

        // add to play fifo
        session->myFifoTimeBase.now();
        myPlayingFifo.addUntil( session, session->myFifoTimeBase );
    }
    else if( session->sessionMode() == RTSP_SESSION_MODE_REC )
    {
        cpLog( LOG_DEBUG, "Adding session %s into recording list",
               session->sessionId().getData(lo) );

        // add to recording list
        myRecordingListMutex.lock();
        myRecordingList.push_front( session );
        myRecordingListMutex.unlock();

        // add to select map
        int fd = session->rtpFd();

        myFdSetMutex.lock();
        FD_SET( fd, &myBaseRecordFd );
        if( fd > myMaxRecordFd )  
        {
            myMaxRecordFd = fd;
        }
        myFdSetMutex.unlock();
    }
    else
    {
        cpLog( LOG_ERR, "Fail: Adding unknown mode to RtspRtpProcessor" );
        assert( 0 );
    }

    cpLog( LOG_ERR, "Now processing %d play and %d record rtp sessions",
           myPlayingFifo.size(), myRecordingList.size() );
    return true;
}


bool
RtspRtpProcessor::delEvent( Sptr<RtspSession> session )
{
    LocalScopeAllocator lo;

    if( session->sessionMode() == RTSP_SESSION_MODE_PLAY )
    {
        // remove session from playing fifo
        cpLog( LOG_DEBUG, "Removing session %s from playing fifo",
               session->sessionId().getData(lo) );
        vusleep( session->myPacketIntervalMs * 1000 );
    }
    else if( session->sessionMode() == RTSP_SESSION_MODE_REC )
    {
        // remove session from recording list
        cpLog( LOG_DEBUG, "Removing session %s from recording list",
               session->sessionId().getData(lo) );

        myFdSetMutex.lock();
        FD_CLR( session->rtpFd(), &myBaseRecordFd );
        myFdSetMutex.unlock();

        myRecordingListMutex.lock();
        myRecordingList.remove( session );
        myRecordingListMutex.unlock();
    }
    else
    {
        cpLog( LOG_ERR, "Deleting unknown session mode from RtspRtpProcessor" );
        return false;
    }

    cpLog( LOG_DEBUG, "Now processing %d play and %d record rtp sessions",
           myPlayingFifo.size(), myRecordingList.size() );
    return true;
}


bool
RtspRtpProcessor::buildRtpPortList( const int low, const int high )
{
    bool result = true;

    if( low >= high || ( ( high - low ) % 2 != 0 ) )
    {
        cpLog( LOG_ERR, "Rtp Port Range valid: %d-%d, using default values",
               low, high );
        assert( ! ( myRtpPortLow >= myRtpPortHigh ||
                  ( ( myRtpPortHigh- myRtpPortLow ) % 2 != 0 ) ) );
        result = false;
    }
    else
    {
        myRtpPortLow = low;
        myRtpPortHigh = high;
    }

    cpLog( LOG_DEBUG, "Rtp Port Range: %d-%d", myRtpPortLow, myRtpPortHigh );
    for( int i = myRtpPortLow; i < myRtpPortHigh; i += 2 )
    {
        myRtpPortSet.insert( i );
    }

    return result;
}


int
RtspRtpProcessor::allocateRtpPort()
{
    if( myRtpPortSet.empty() )
    {
        return 0;
    }
    int result = *myRtpPortSet.begin();
    myRtpPortSet.erase( myRtpPortSet.begin() );

    cpLog( LOG_DEBUG, "Rtp port %d allocated", result );
    return result;
}


bool
RtspRtpProcessor::deallocateRtpPort( const int port )
{
    if( port >= myRtpPortLow  &&  port <= myRtpPortHigh )
    {
        cpLog( LOG_DEBUG, "Rtp port %d deallocated", port );
        myRtpPortSet.insert( port );
        return true;
    }
    return false;
}


void*
RtspRtpProcessor::runPlayThread()
{
    LocalScopeAllocator lo;
    cpLog( LOG_DEBUG, "RtspRtpProcessor play thread running" );

    Sptr<RtspSession> session;
    Sptr<State> PLAY_STATE =
                StateMachine::instance().findState( "StatePlaying", 1 );

    myPlayBufferMax = 1024;
    myPlayBuffer = new char[myPlayBufferMax];

    while( 1 )
    {
        session = myPlayingFifo.getNext();
        if( session == 0 )
        {
            // check if stop processing
            myShutdownMutex.lock();
            bool shutdownNow = myShutdown;
            myShutdownMutex.unlock();
            if( shutdownNow )
            {
                break;
            }
            else
            {
                assert( 0 );
            }
        }

        if( cpLogGetPriority() >= LOG_DEBUG_HB )
        {
            //cerr<<"play";
        }

        if( session->myState != PLAY_STATE )
        {
            cpLog( LOG_DEBUG,
                   "Server trying to remove session from play fifo" );
            continue;
        } 

        // process rtp packet
        if( !playRtpPacket( session ) )
        {
            cpLog( LOG_DEBUG, "Fail in playRtpPacket" );
            continue;
        }

        session->myCurrentNpt += session->myPacketIntervalMs;
        session->myPacketCounter++;

        // check for pause request
        if( session->myPausePoint != -1  &&
            session->myPausePoint <= session->myCurrentNpt )
        {
            OpPause::processPendingEvent( session->pendingPause() );

            // check for pending play requests for immediate unpause
            if( session->myPendingEvents.messageAvailable() )
            {
                OpPlay::processPendingEvent(
                        session->myPendingEvents.getNext() );
            }
        }

        // check if done sending
        if( session->myPacketTotal == -1  ||
            session->myPacketCounter < session->myPacketTotal )
        {
            // add next packet event into fifo
            milliseconds_t delay = session->myPacketIntervalMs *
                                   session->myPacketCounter;
            TimeVal fifoTime = session->myFifoTimeBase + delay;
            myPlayingFifo.addUntil( session, fifoTime );
        }
        else
        {
            // This play request is done, update statistics
            if (RtspConfiguration::instance().logStatistics)
            {
                session->myPerPlayReqData.myBytesSent = 
                    session->myPerPlayReqData.myPktsSent *
                    session->fileHandler()->packetSize();
                session->myPerPlayReqData.myStreamWaitSent.stop();
                if (session->myPerPlayReqData.myPktsSent > 0)
                {
                    session->myStatsMutex.lock();
                    session->myStats.myPlayDataList.push_front(session->myPerPlayReqData);
                    session->myStatsMutex.unlock();
                    session->myPerPlayReqData.myPktsSent = 0;
                }
            }

            // start next play request
            if( session->myPendingEvents.messageAvailable() )
            {
                OpPlay::processPendingEvent(
                        session->myPendingEvents.getNext() );
            }
            else
            {
                cpLog( LOG_DEBUG,
                       "Done processing rtp packets for %s at npt %d",
                       session->sessionId().getData(lo),
                       session->myCurrentNpt );
                session->myPausePoint = - 1;
                session->state( StateMachine::instance().
                                findState( "StatePausePlay" ) );
            }
        }
    }

    return 0;
}


bool
RtspRtpProcessor::playRtpPacket( Sptr<RtspSession> session )
{
    int cc = session->myFileHandler->read( myPlayBuffer, myPlayBufferMax );
    if( cc > 0 )
    {
        if( !session->myRtpSession->transmitRaw( myPlayBuffer, cc ) )
        {
            cpLog( LOG_DEBUG, "Error in myRtpSession->transmit()", cc );
        }
        else
        {
            if (RtspConfiguration::instance().logStatistics)
            {
                session->myPerPlayReqData.myPktsSent++;
            }
        }
        return true;
    }
    else if( cc == 0 )
    {
        cpLog( LOG_DEBUG, "End of file reached" );

        // modify values to correct debug printing
        session->myPacketTotal = session->myPacketCounter;
        session->myCurrentNpt -= session->myPacketIntervalMs;
        return true;
    }
    else // cc < 0
    {
        cpLog( LOG_DEBUG, "Error in myFileHandler.read(), %d", cc );
        return false;
    }
}


bool
RtspRtpProcessor::playThreadLoaded() 
{
    if( (int) myPlayingFifo.size() > MIN_HANDLING_LOAD )
    {
        if( myPlayingFifo.sizePending() == 0 )
        {
            return true;
        }
    }
    return false;
}


void*
RtspRtpProcessor::runRecordThread()
{
    cpLog( LOG_DEBUG, "RtspRtpProcessor record thread running" );

    Sptr<State> RECORD_STATE =
                StateMachine::instance().findState( "StateRecording", 1 );

    myRecBufferMax = 1024;
    myRecBuffer = new char[myRecBufferMax];

    myMaxRecordFd = 0;
    FD_ZERO( &myBaseRecordFd );
    struct timeval recTimeout;
    int selectResult;
    myPrevSelectResult = 0;

    while( 1 )
    {
        // setup select parameters
        myFdSetMutex.lock();
        fd_set recordFd = myBaseRecordFd;
        myFdSetMutex.unlock();
        recTimeout.tv_sec = 1;
        recTimeout.tv_usec = 0;

        selectResult = select( myMaxRecordFd+1, &recordFd, 0, 0, &recTimeout );
        if( cpLogGetPriority() >= LOG_DEBUG_HB )
        {
            //cerr<<"rec";
        }
        if( selectResult < 0 )
        {
            //TODO handle select errors better
            //TODO pass EINT as continue;
            cpLog( LOG_DEBUG, "Select() error in runRecordThread" );
        }
        else if( selectResult == 0 )
        {
            // check if stop processing
            myShutdownMutex.lock();
            bool shutdownNow = myShutdown;
            myShutdownMutex.unlock();
            if( shutdownNow )
            {
                break;
            }
            else
            {
                continue;
            }
        }

        myPrevSelectResult = selectResult;

        // process some sessions
        myRecordingListMutex.lock();
        for( RecordingList::iterator itr = myRecordingList.begin();
             itr != myRecordingList.end(); itr++ )
        {
            if( FD_ISSET( (*itr)->rtpFd(), &recordFd ) )
            {
                Sptr<RtspSession> session = (*itr);
                if( session->myState == RECORD_STATE )
                {
                    // process rtp packet
                    recordRtpPacket( session );

                    // check for pause request
                    if( session->myPausePoint != -1  &&
                        session->myPausePoint <= session->myCurrentNpt )
                    {
                        OpPause::processPendingEvent( session->pendingPause() );
                    }
                }
                else
                {
                    cpLog( LOG_DEBUG,
                      "Server trying to remove session from recording list" );
                }
            }
        }
        myRecordingListMutex.unlock();
    }

    delete []myRecBuffer;
    myRecBuffer = 0;

    return 0;
}


void
RtspRtpProcessor::recordRtpPacket ( Sptr<RtspSession> session )
{
    myRtpPacket = session->myRtpSession->receive();
    if( RtspConfiguration::instance().logStatistics && !myRtpPacket )
    {
        if( !session->myPerRecordReqData.myFirstPktsFlag && myRtpPacket )
        {
            session->myPerRecordReqData.myFirstPktsFlag = true;
            session->myPerRecordReqData.myPreStreamWaitRecvd.stop();
            session->myPerRecordReqData.myStreamWaitRecvd.start();
        }
    }

    while( myRtpPacket )
    {
        session->myFileHandler->write( myRtpPacket->getPayloadLoc(),
                                       myRtpPacket->getPayloadUsage(),
                                       myRtpPacket->getSequence(),
                                       myRtpPacket->getRtpTime() );
        //TODO check result of write call
        //TODO check record bytes haven't exceeded max
        // max = RtspConfiguration::instance().maxRecordingFileBytes
        delete myRtpPacket;
        session->myCurrentNpt += session->myPacketIntervalMs;
        session->myPacketCounter++;

        if (RtspConfiguration::instance().logStatistics)
        {
            session->myPerRecordReqData.myPktsRecvd++;
        }

        myRtpPacket = session->myRtpSession->receive();
        if (myRtpPacket == NULL) 
        { 
//            vusleep(session->myPacketIntervalMs * 1000);
            myRtpPacket = session->myRtpSession->receive();
        }
    }
}


bool
RtspRtpProcessor::recordThreadLoaded() 
{
    int recSize = myRecordingList.size();
    if( recSize > MIN_HANDLING_LOAD )
    {
        if( myPrevSelectResult == recSize )
        {
            return true;
        }
    }
    return false;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
