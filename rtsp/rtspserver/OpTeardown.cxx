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

static const char* const OpTeardown_cxx_version =
    "$Id: OpTeardown.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "OpTeardown.hxx"
#include "RtspServer.hxx"
#include "RtspSessionManager.hxx"
#include "RtspRtpProcessor.hxx"
#include "RtspTeardownMsg.hxx"


const Sptr<State>
OpTeardown::process( const Sptr<StateEvent> event )
{
    Sptr<RtspMsg> origMsg = event->getRtspMsg();
    Sptr<RtspTeardownMsg> msg;
    msg.dynamicCast(origMsg);

    if( msg == 0 ) 
    {
        return NEXT_STATE;
    }

    cpLog( LOG_DEBUG, "Handling TEARDOWN" );

    Sptr<RtspSession> session = RtspSessionManager::instance().
                                getRtspSession( msg->getSessionId() );

    // teardown session
    process2( session );

#ifndef NO_200_FOR_TEARDOWN
    // build and send response
    Sptr<RtspResponse> resp = new RtspResponse( msg, RTSP_200_STATUS );
    resp->appendConnectionCloseHdr();
    RtspServer::instance().sendResponse( resp );
#endif

    return DONE_STATE;
}


void
OpTeardown::process2( Sptr<RtspSession> session )
{
    // notify RtspRtpProcessor to stop processing events for this session
    if( session->sessionMode() != RTSP_SESSION_MODE_UNKNOWN )
    {
        LocalScopeAllocator lo;

        cpLog( LOG_DEBUG, "Stopping rtp processor for session %s",
               session->sessionId().getData(lo) );
        session->state( StateMachine::instance().findState( "StateInit" ) );
        RtspRtpProcessor::instance().delEvent( session );
    }

    cpLog( LOG_ERR, "packetCounter = %d", session->myPacketCounter );

    if( session->sessionMode() == RTSP_SESSION_MODE_REC )
    {
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
    
            RtspServer::instance().myStatsMutex.lock();

            RtspServer::instance().myStats.myTotalActRecSessions--;
            RtspServer::instance().myStats.myTotalFinRecSessions++;
    
            RtspServer::instance().myStats.myTotalRecReqProcessed +=
                session->myStats.totalRecRequests();
            RtspServer::instance().myStats.myTotalRecThroughput += 
                session->myStats.totalRecThroughput();
            RtspServer::instance().myStats.myTotalRecRoughThroughput += 
                session->myStats.totalRecRoughThroughput();
            RtspServer::instance().myStats.myTotalRecPSW +=
                session->myStats.totalRecPSW();
    
            unsigned int a = session->myStats.totalPktsRecvd();
            unsigned int b = session->myStats.totalBytesRecvd();
    
            minMaxPktsBytes(a, b);

            RtspServer::instance().myStatsMutex.unlock();

        }

        cpLog( LOG_ERR, "Udp received pkts: %d",
               session->rtpSession()->getRtpRecv()->getUdpStack()->getPacketsReceived() );
    }

    if( session->sessionMode() == RTSP_SESSION_MODE_PLAY )
    {
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
    
            RtspServer::instance().myStatsMutex.lock();

            RtspServer::instance().myStats.myTotalActPlySessions--;
            RtspServer::instance().myStats.myTotalFinPlySessions++;
    
            RtspServer::instance().myStats.myTotalPlyReqProcessed +=
                session->myStats.totalPlyRequests();
            RtspServer::instance().myStats.myTotalPlyThroughput += 
                session->myStats.totalPlyThroughput();
    
            unsigned int a = session->myStats.totalPktsSent();
            unsigned int b = session->myStats.totalBytesSent();
    
            minMaxPktsBytes(a, b);

            RtspServer::instance().myStatsMutex.unlock();

        }
    }

    // deallocate resources
    session->deleteRtpStack();
    RtspRtpProcessor::instance().deallocateRtpPort( session->transport().
                                                    myServerPortA );

    if (RtspConfiguration::instance().logStatistics)
    {
        LocalScopeAllocator lo;

        session->myStats.report(session->sessionId().getData(lo));
        if( !RtspServer::instance().myStats.report() )
        {
            RtspConfiguration::instance().logStatistics = false;
        }
    }

    session->fileHandler()->closeAudioFile();

    RtspSessionManager::instance().delRtspSession( session->sessionId() );
}

void
OpTeardown::minMaxPktsBytes(unsigned int a, unsigned int b)
{
    if ( a < RtspServer::instance().myStats.myMinPktsStreamedPerSession )
        RtspServer::instance().myStats.myMinPktsStreamedPerSession = a;

    if ( a > RtspServer::instance().myStats.myMaxPktsStreamedPerSession )
        RtspServer::instance().myStats.myMaxPktsStreamedPerSession = a;

    if ( b < RtspServer::instance().myStats.myMinBytesStreamedPerSession )
        RtspServer::instance().myStats.myMinBytesStreamedPerSession = b;

    if ( b > RtspServer::instance().myStats.myMaxBytesStreamedPerSession )
        RtspServer::instance().myStats.myMaxBytesStreamedPerSession = b;

}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
