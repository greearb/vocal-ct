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

static const char* const OpRecord_cxx_version =
    "$Id: OpRecord.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "OpRecord.hxx"
#include "RtspServer.hxx"
#include "RtspSessionManager.hxx"
#include "RtspRtpProcessor.hxx"

//check sessionId matches file in fileHandler

const Sptr<State>
OpRecord::process( const Sptr<StateEvent> event )
{
    Sptr<RtspMsg> origMsg = event->getRtspMsg();
    Sptr<RtspRecordMsg> msg;
    msg.dynamicCast(origMsg);

    if( msg == 0 )
    {
        return NEXT_STATE;
    }

    cpLog( LOG_DEBUG, "Handling RECORD" );

    Sptr<RtspSession> session = RtspSessionManager::instance().
                                getRtspSession( msg->getSessionId() );
    assert( session != 0 );

    // recording reqested file
    if( session->state() != StateMachine::instance().findState( "StateRecording", true ) )
    {

        LocalScopeAllocator lo;
        cpLog( LOG_DEBUG, "Recording file: %s",
               session->fileHandler()->filename().getData(lo) );
        processPendingEvent( msg );
    }

    return DONE_STATE;
}


void
OpRecord::processPendingEvent( Sptr<RtspRecordMsg> event )
{
    Sptr<RtspSession> session = RtspSessionManager::instance().
                                getRtspSession( event->getSessionId() );
    assert( session != 0 );

    LocalScopeAllocator lo;

    cpLog( LOG_DEBUG, "Processing record request for session %s",
           session->sessionId().getData(lo) );

    // process range header
    Sptr<RtspRangeHdr> range = event->getRange();
    if( range != 0 )
    {
        cpLog( LOG_DEBUG, "Range found, but not supported for recording, reply 457" );
        Sptr<RtspResponse> response = new RtspResponse( event, RTSP_457_STATUS );
        RtspServer::instance().sendResponse( response );
        return;
    }

    // adjust new start and end times
    if( session->state() == StateMachine::instance().findState( "StatePauseRecord", true ) )
    {   
        session->myPausePoint = -1;
        cpLog( LOG_DEBUG, "Unpausing at npt %d", session->myCurrentNpt );
    }
    else
    {
        session->myPausePoint = -1;
        session->myCurrentNpt = 0;
        session->fileHandler()->seek( 0 );
        session->myPacketTotal = -1;
        session->myPacketCounter = 0;
    }

    cpLog( LOG_DEBUG, "Recording ? %dbyte packets at %dms intervals",
           session->fileHandler()->packetSize(), session->myPacketIntervalMs );

    // build and send response
    Sptr<RtspResponse> resp = new RtspResponse( event, RTSP_200_STATUS );
    RtspServer::instance().sendResponse( resp );

    // add statistic data
    if (RtspConfiguration::instance().logStatistics)
    {
        session->myPerRecordReqData.myPktsRecvd = 0;
        session->myPerRecordReqData.myBytesRecvd = 0;
        session->myPerRecordReqData.myPreStreamWaitRecvd.start();
        session->myPerRecordReqData.myFirstPktsFlag = false;
    }

    // move into record state
    session->state( StateMachine::instance().findState( "StateRecording" ) );

    // add session into rtp processor
    RtspRtpProcessor::instance().addInitalEvent( session );

    return;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
