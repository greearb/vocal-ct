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

static const char* const OpSetup_cxx_version =
    "$Id: OpSetup.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "OpSetup.hxx"
#include "RtspServer.hxx"
#include "RtspSessionManager.hxx"
#include "RtspRtpProcessor.hxx"
#include "RtspSetupMsg.hxx"
#include "RtspServerStats.hxx"

//check file requested is still valid or has been recently described

const Sptr<State>
OpSetup::process( const Sptr<StateEvent> event )
{
    Sptr<RtspMsg> origMsg = event->getRtspMsg();
    Sptr<RtspSetupMsg> msg;
    msg.dynamicCast(origMsg);

    if( msg == 0 )
    {
        return NEXT_STATE;
    }

    cpLog( LOG_DEBUG, "Handling SETUP" );

    RtspServer& rtspServer = RtspServer::instance();

    // create new RtspSession
    Sptr<RtspSession> newSession = new RtspSession();
    Sptr<RtspTransportSpec> transport = msg->getTransport();

    // build content base
    Data contentBase = rtspServer.networkAddress().getIpName();
    contentBase += ":";
    contentBase += rtspServer.networkAddress().getPort();
    contentBase += "/";

    // open reqested file
    Data filename = msg->getFilePath();
    LocalScopeAllocator lo;

    cpLog( LOG_DEBUG, "Opening file %s", filename.getData(lo) );

    // blocksize can be zero, openAudioFile will give default
    Sptr<RtspFileHandler> rtspFd = new RtspFileHandler( filename, contentBase );
    if( !rtspFd->openAudioFile( transport->myIsRecord, msg->getBlocksize() ) )
    {
        cpLog( LOG_DEBUG, "Fail to open file, reply 404" );
        Sptr<RtspResponse> response = new RtspResponse( msg, RTSP_404_STATUS );
        rtspServer.sendResponse( response );
        return DONE_STATE;
    }

    // save rtp transport information into session
    int rtpPort = RtspRtpProcessor::instance().allocateRtpPort();
    if( !rtpPort )
    {
        cpLog( LOG_DEBUG, "No more available RTP ports, reply 453" );
        Sptr<RtspResponse> response = new RtspResponse( msg, RTSP_453_STATUS );
        rtspServer.sendResponse( response );
        return DONE_STATE;
    }
    transport->myServerPortA = rtpPort;
    transport->myServerPortB = rtpPort + 1;
    transport->mySource = rtspServer.networkAddress().getIpName();

    // attach file handler and transport
    Connection conn = msg->getTransConnPtr()->getConnection();
    NetworkAddress clientAddr( conn.getDescription() );
    newSession->clientAddress( clientAddr );
    newSession->serverAddress( rtspServer.networkAddress() );
    newSession->transport( *transport );
    newSession->fileHandler( rtspFd );
    newSession->myPacketIntervalMs = rtspFd->packetInterval();

    // get codec and codecString from the sdp block
    int sdpCodec = -1;
    string sdpCodecString = "UNKNOWN";
    if (msg->getMsgBody().length() > 0)
    {
        RtspSdp rtspSdp( msg->getMsgBody() );
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
       if (sdpCodecString == rtpFileTypeInfo[newSession->fileHandler()->ftIndex()].name)
       {
           newSession->fileHandler()->setDynamicCodec( sdpCodec );
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

    // create rtp stack and set session mode
    if( !newSession->createRtpStack( transport->myIsRecord ) )
    {
        cpLog( LOG_DEBUG, "Failure to create RTP stack, reply 453" );
        Sptr<RtspResponse> response = new RtspResponse( msg, RTSP_453_STATUS );
        rtspServer.sendResponse( response );
        return DONE_STATE;
    }
    
    // save setup message
    newSession->setupMsg( msg );

    // add RtspSession into server session map
    Data sessionId = RtspSessionManager::instance().addRtspSession(newSession);
    if( sessionId == "" )
    {
        cpLog( LOG_DEBUG, "No more available session, failing");
        Sptr<RtspResponse> response = new RtspResponse( msg, RTSP_453_STATUS );
        rtspServer.sendResponse( response );
        return DONE_STATE;
    }

    // build and send response
    Sptr<RtspResponse> resp = new RtspResponse( msg, RTSP_200_STATUS );
    resp->appendSessionId( sessionId.convertInt() );
    resp->appendTransportHdr( transport );
    if (appendSdp)
        resp->appendSdpData( respSdp );
    rtspServer.sendResponse( resp );

    if (RtspConfiguration::instance().logStatistics)
    {
        newSession->myStatsMutex.lock();
        newSession->myStats.myCodecUsed = newSession->fileHandler()->ftIndex();
        newSession->myStatsMutex.unlock();
        if ((newSession->myStats.myCodecUsed >= 0) && 
            (newSession->myStats.myCodecUsed < FileTypeIndex))
        {
            RtspServer::instance().myStatsMutex.lock();
            (RtspServer::instance().myStats.myCodecUsage
             [newSession->myStats.myCodecUsed])++;
            RtspServer::instance().myStatsMutex.unlock();
        }
    }

    switch( newSession->sessionMode() )
    {
        case RTSP_SESSION_MODE_PLAY:
        {
            if (RtspConfiguration::instance().logStatistics)
            {
                RtspServer::instance().myStatsMutex.lock();
                RtspServer::instance().myStats.myTotalActPlySessions++;
                RtspServer::instance().myStatsMutex.unlock();
            }

            newSession->state( StateMachine::instance().
                               findState( "StateReadyPlay" ) );
            cpLog( LOG_ERR, "Begin Playing new session" );
            break;
        }
        case RTSP_SESSION_MODE_REC:
        {
            if (RtspConfiguration::instance().logStatistics)
            {
                RtspServer::instance().myStatsMutex.lock();
                RtspServer::instance().myStats.myTotalActRecSessions++;
                RtspServer::instance().myStatsMutex.unlock();
            }

            newSession->state( StateMachine::instance().
                               findState( "StateReadyRecord" ) );
            cpLog( LOG_ERR, "Begin Recording new session" );
            break;
        }
        default:
            assert( 0 );
    }

    return DONE_STATE;
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
