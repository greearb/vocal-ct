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

static const char* const OpPlay_cxx_version =
    "$Id: OpPlay.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "OpPlay.hxx"
#include "RtspServer.hxx"
#include "RtspSessionManager.hxx"
#include "RtspRtpProcessor.hxx"
#include "RtspPlayMsg.hxx"
#include "RtspRtpInfoHdr.hxx"

//check sessionId matches file in fileHandler

const Sptr<State>
OpPlay::process( const Sptr<StateEvent> event )
{
    Sptr<RtspMsg> origMsg = event->getRtspMsg();
    Sptr<RtspPlayMsg> msg;
    msg.dynamicCast(origMsg);

    if( msg == 0 )
    {
        return NEXT_STATE;
    }

    cpLog( LOG_DEBUG, "Handling PLAY" );

    Sptr<RtspSession> session = RtspSessionManager::instance().
                                getRtspSession( msg->getSessionId() );
    assert( session != 0 );

    LocalScopeAllocator lo;


    // play reqested file
    cpLog( LOG_DEBUG, "Playing file: %s", session->fileHandler()->filename().getData(lo) );
    if( session->state() != StateMachine::instance().findState( "StatePlaying", true ) )
    {
        processPendingEvent( msg );
    }
    else
    {
        // 200 reponse will come when RtspRtpProcessor handles play request
        cpLog( LOG_DEBUG, "Adding Play request into pending play fifo" );
        session->addPendingEvent( msg );
    }

    return DONE_STATE;
}


void
OpPlay::processPendingEvent( Sptr<RtspPlayMsg> event )
{
    Sptr<RtspSession> session = RtspSessionManager::instance().
                                getRtspSession( event->getSessionId() );
    assert( session != 0 );


    LocalScopeAllocator lo;

    cpLog( LOG_DEBUG, "Processing play request for session %s",
           session->sessionId().getData(lo) );

    long startTime;
    long endTime;

    // process range header
    Sptr<RtspRangeHdr> range = event->getRange();
    if( range == 0 )
    {
        // default range from beginning to end
        startTime = 0;
        endTime = session->fileHandler()->length();
    }
    else
    {
        // range found
        startTime = event->getRange()->getStartTime();
        endTime = event->getRange()->getEndTime();

        // this is same as no range header
        if( startTime != -1 && endTime != -1 )
        {
            if( startTime < 0 )
            {
                startTime = 0;
            }
            if( endTime < 0 )
            {
                endTime = session->fileHandler()->length();
            }
        }
    }

#ifdef INFINITE_READ
    endTime = -1;
#endif

    // adjust new start and end times
    if( session->state() == StateMachine::instance().findState( "StatePausePlay", true )  &&
        range == 0  &&  session->myPausePoint != -1 )
    {   
        session->myPausePoint = -1;
        session->myPacketTotal = ( session->myPacketTotal == -1 ) ? -1 :
                                   session->myPacketTotal - session->myPacketCounter;
        session->myPacketCounter = 0;
        cpLog( LOG_DEBUG, "Unpausing at npt %d", session->myCurrentNpt );
    }
    else
    {
        session->myPausePoint = -1;
        session->myCurrentNpt = startTime;
        session->fileHandler()->seek( startTime );
        session->myPacketTotal = ( endTime == -1 ) ?  -1 :
                                 ( endTime - startTime ) / session->myPacketIntervalMs;
        session->myPacketCounter = 0;
        if( endTime != -1 )
            cpLog( LOG_DEBUG, "Playing new npt interval %d - %d", startTime, endTime );
        else
            cpLog( LOG_DEBUG, "Playing new npt interval %d - <unknown>", startTime ); 
    }

    cpLog( LOG_DEBUG, "Playing %d %dbyte packets at %dms intervals",
           session->myPacketTotal, session->fileHandler()->packetSize(),
           session->myPacketIntervalMs );

    // get codec and codecString from the sdp block
    int sdpCodec = -1;
    string sdpCodecString = "UNKNOWN";
    if (event->getMsgBody().length() > 0)
    {
        RtspSdp rtspSdp( event->getMsgBody() );
        rtspSdp.getCodecNCodecString( sdpCodec, sdpCodecString );
    }
    else
    {
        cpLog(LOG_DEBUG, "no sdp info.");
    }

    // compare sdpCodec and the audio file codec info
    bool appendSdp = false;
    if (sdpCodec >= rtpPayloadDynMin && sdpCodec <= rtpPayloadDynMax)
    {
       if (sdpCodecString == rtpFileTypeInfo[session->fileHandler()->ftIndex()].name)
       {
           session->fileHandler()->setDynamicCodec( sdpCodec );
           session->rtpSession()->setApiFormat((RtpPayloadType)sdpCodec,
                                  session->fileHandler()->packetSamples(),
                                  session->fileHandler()->packetSize());
           session->rtpSession()->setNetworkFormat((RtpPayloadType)sdpCodec,
                                  session->fileHandler()->packetSamples(),
                                  session->fileHandler()->packetSize());

           cpLog(LOG_DEBUG, "The dynamic Codec is: %d", sdpCodec);
           appendSdp = true;
       }
       else
       {
           cpLog(LOG_DEBUG, "sdp codec string is not match with audio file.");
       }
    }

    RtspSdp respSdp;
    if (appendSdp)
    {
        respSdp.setSessionName( RtspConfiguration::instance().serverName );
        respSdp.setMediaPort( 0 );
        respSdp.addMediaFormat( sdpCodec );
        respSdp.addRtpmap( sdpCodec, sdpCodecString, 8000);
    }

    // build and send response
    Sptr<RtspResponse> resp = new RtspResponse( event, RTSP_200_STATUS );

    Data filenameUrl( session->fileHandler()->filenameUrl() );
    Sptr<RtspRtpInfoHdr> rtspInfo = new RtspRtpInfoHdr( filenameUrl );

    rtspInfo->mySeq = session->rtpSession()->getPrevSequence();
    rtspInfo->myRtptime = session->rtpSession()->getPrevRtpTime();
    resp->appendRtpInfoHdr( rtspInfo );

    session->rtpSession()->setMarkerOnce();
    if (appendSdp)
        resp->appendSdpData( respSdp );
    RtspServer::instance().sendResponse( resp );

    // add statistic data
    if (RtspConfiguration::instance().logStatistics)
    {
        session->myPerPlayReqData.myPktsSent = 0;
        session->myPerPlayReqData.myBytesSent = 0;
        session->myPerPlayReqData.myStreamWaitSent.start();
    }

    // move into playing state
    session->state( StateMachine::instance().findState( "StatePlaying" ) );

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
