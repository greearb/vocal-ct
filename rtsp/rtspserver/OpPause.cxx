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

static const char* const OpPause_cxx_version =
    "$Id: OpPause.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "OpPause.hxx"
#include "RtspServer.hxx"
#include "RtspSessionManager.hxx"
#include "RtspPauseMsg.hxx"


//TODO should we send 100 Trying, since pause wouldn't get 200 right away
const Sptr<State>
OpPause::process(const Sptr<StateEvent> event)
{
    Sptr<RtspMsg> origMsg = event->getRtspMsg();
    Sptr<RtspPauseMsg> msg;
    msg.dynamicCast(origMsg);

    if( msg == 0 )
    {
        return NEXT_STATE;
    }

    cpLog( LOG_DEBUG, "Handling PAUSE" );

    // get session object
    Sptr<RtspSession> session = RtspSessionManager::instance().
                                getRtspSession( msg->getSessionId() );
    assert( session != 0 );


    LocalScopeAllocator lo;

    cpLog( LOG_DEBUG, "Processing pause request for session %s",
           session->sessionId().getData(lo) );

    // set pause point in session
    Sptr<RtspRangeHdr> range = msg->getRange();
    if( range == 0 )
    {
        session->myPausePoint = session->myCurrentNpt;
    }
    else
    {
        session->myPausePoint = range->getStartTime();
    }
    cpLog( LOG_DEBUG, "Setting pause point at %d", session->myPausePoint );
    // save message so when pause point occurs, rtsp server can send response
    session->pendingPause( msg );

    return DONE_STATE;
}


void
OpPause::processPendingEvent( Sptr<RtspPauseMsg> event )
{
    Sptr<RtspSession> session = RtspSessionManager::instance().
                                getRtspSession( event->getSessionId() );
    assert( session != 0 );


    LocalScopeAllocator lo;

    cpLog( LOG_DEBUG, "Pausing session %s at %d",
           session->sessionId().getData(lo), session->myPausePoint );

    // clear pause msg
    // pause point isn't reset to -1 because pause state can be auto set
    session->pendingPause( 0 );

    // build and send response
    Sptr<RtspResponse> resp = new RtspResponse( event, RTSP_200_STATUS );
    RtspServer::instance().sendResponse( resp );

    // move into pause state
    switch( session->sessionMode() )
    {
        case RTSP_SESSION_MODE_PLAY:
        {
            session->state( StateMachine::instance().findState( "StatePausePlay" ) );
            //update statistic
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
            break;
        }
        case RTSP_SESSION_MODE_REC:
        {
            session->state( StateMachine::instance().findState( "StatePauseRecord" ) );
            //update statistic
            if (RtspConfiguration::instance().logStatistics)
            {
                session->myPerRecordReqData.myBytesRecvd = 
                    session->fileHandler()->packetSize() * 
                    session->myPerRecordReqData.myPktsRecvd;
                session->myPerRecordReqData.myStreamWaitRecvd.stop();
                if (session->myPerRecordReqData.myPktsRecvd > 0)
                {
                    session->myStatsMutex.lock();
                    session->myStats.myRecordDataList.push_front(session->myPerRecordReqData);
                    session->myStatsMutex.unlock();
                    session->myPerRecordReqData.myPktsRecvd = 0;
                }
            }
            break;
        }
        default:
            assert( 0 );
    }

    return;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
