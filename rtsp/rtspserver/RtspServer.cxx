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

static const char* const RtspServer_cxx_Version =
    "$Id: RtspServer.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspServer.hxx"
#include "RtspConfiguration.hxx"
#include "RtspSessionManager.hxx"
#include "RtspRtpProcessor.hxx"
#include "OpTeardown.hxx"

// singelton object definition
RtspServer* RtspServer::myInstance = 0;


RtspServer&
RtspServer::instance( const int port )
{
    if( myInstance == 0 )
    {
        myInstance = new RtspServer( port );
    }
    return *myInstance;
}


RtspServer::RtspServer( const int port )
    : myMsgInterface( port )
{
#if defined (_linux_)    
    // register destroy function to delete singelton
    if( atexit( RtspServer::destroy ) < 0 )
    {
        cpLog( LOG_ALERT, "Failed to register with atexit()" );
    }
#endif

    // save hostname into networkAddress
    char hostName[256];
    if( gethostname( hostName, 255 ) != 0 )
    {
        cpLog( LOG_ALERT, "Failed to get the RTSP server host name, reason %s",
               strerror( errno ) );
        assert( 0 );
    }
    myNetworkAddress.setHostName( hostName );
    myNetworkAddress.setPort( port );

    // create singleton of state machine
    StateMachine& stateMachine = StateMachine::instance();
    cpLog( LOG_DEBUG, "%s created", stateMachine.name() );

    // read configuration file
    RtspConfiguration& cfg = RtspConfiguration::instance();

    // limit number of supported sessions
    RtspSessionManager::instance().maxSessions( cfg.maxSessions );

    // build rtp processor
    RtspRtpProcessor::instance().buildRtpPortList( cfg.rtpPortRangeMin,
                                                   cfg.rtpPortRangeMax );

    // start rtp processor thread
    RtspRtpProcessor::instance().startThread();
}


RtspServer::~RtspServer()
{
}


void
RtspServer::destroy()
{
    delete RtspServer::myInstance;
    RtspServer::myInstance = 0;
}


void
RtspServer::run()
{
    cpLog( LOG_DEBUG, "RtspServer thread is running" );

    while( 1 )
    {
        // get next rtsp message (blocking)
        Sptr<RtspMsg> msg = myMsgInterface.getNext();

        cpLog( LOG_DEBUG, "Processing new msg from fifo" );

        Sptr<RtspRequest> request;
        request.dynamicCast(msg);

        // check if msg is a rtsp request
        if( request == 0 )
        {
            cpLog( LOG_DEBUG, "Non request rtsp message received, ignoring" );
            continue;
        }
        else
        {
            if (RtspConfiguration::instance().logStatistics)
            {
                myStatsMutex.lock();
                myStats.myTotalReqRecvd++;
                myStatsMutex.unlock();
            }

            // this has to be called to trigger the parsing for startline
            u_int32_t mthd = request->getMethod();
            if( ( mthd == RTSP_UNKNOWN_MTHD ) || ( mthd == RTSP_NULL_MTHD ) )
            {
                // throw away this msg
                if (RtspConfiguration::instance().logStatistics)
                {
                    myStatsMutex.lock();
                    myStats.myTotalNonSuccessReqRecvd++;
                    myStatsMutex.unlock();
                }
                continue;
            }
        }

        // process session request differently
        if( request->isSessionRequest() )
        {
            if( !processRequest( request ) )
            {
                break;
            }
        }
        else
        {
            if( !processSessionRequest( request ) )
            {
                break;
            }
        }
    }

    stop();
    cpLog( LOG_DEBUG, "RtspServer thread is stopped" );
    return;
}


void
RtspServer::stop( const int delay )
{
    cpLog( LOG_DEBUG, "Shutting down rtsp server" );

    RtspSessionManager& sessionManager = RtspSessionManager::instance();

    // teardown and shutdown each session
    Sptr<RtspSession> session = sessionManager.getRtspSession();
    while( session != 0 )
    {
        // send teardown to client
        //TODO
        //sendRequest( msg );

        // teardown session
        OpTeardown::process2( session );

        // set next while() iterator
        session = sessionManager.getRtspSession();
    }

    // reject all other messages in msg fifo
    while( myMsgInterface.messageAvailable() )
    {
        Sptr<RtspMsg> msg = myMsgInterface.getNext();

        Sptr<RtspRequest> request;
        request.dynamicCast(msg);

        if( request != 0 )
        {
            cpLog( LOG_DEBUG, "Server going down, reply 503" );

            Sptr<RtspResponse> response = new RtspResponse( request, RTSP_503_STATUS );
            sendResponse( response );
        }
    }

    // stop rtp processor thread
    RtspRtpProcessor::instance().stopThread();

    // stop tcp connection thread
    //TODO
}



bool
RtspServer::processRequest( Sptr<RtspRequest> msg )
{
    cpLog( LOG_DEBUG, "Processing non-session request" );

    StateMachine& stateMachine = StateMachine::instance();

    Sptr<StateEvent> newEvent = new StateEvent( msg );
    Sptr<State> startState = stateMachine.findState( "StateInit" );
    Sptr<State> resultState = stateMachine.process( startState, newEvent );

    if( resultState == NEXT_STATE )
    {
        if (RtspConfiguration::instance().logStatistics)
        {
            myStatsMutex.lock();
            myStats.myTotalNonSuccessReqRecvd++;
            myStatsMutex.unlock();
        }

        cpLog( LOG_DEBUG, "Unknown RTSP method, reply 400" );
        Sptr<RtspResponse> response = new RtspResponse( msg, RTSP_400_STATUS );
        sendResponse( response );
        return true;
    }
    else if( resultState == FATAL_STATE )
    {
        cpLog( LOG_DEBUG, "Fatal state occured, shutdown server" );
        return false;
    }

    return true;
}


bool
RtspServer::processSessionRequest( Sptr<RtspRequest> msg )
{
    cpLog( LOG_DEBUG, "Processing session request" );

    Sptr<RtspSession> session = RtspSessionManager::instance().
                                getRtspSession( msg->getSessionId() );
    if( session == 0 )
    {
        if( msg->getSessionId() == "Global" )
        {
            return processServerRequest( msg );
        }
 
        if (RtspConfiguration::instance().logStatistics)
        {
            myStatsMutex.lock();
            myStats.myTotalNonSuccessReqRecvd++;
            myStatsMutex.unlock();
        }

        cpLog( LOG_DEBUG, "SessionId not found, reply 454" );
        Sptr<RtspResponse> response = new RtspResponse( msg, RTSP_454_STATUS );
        sendResponse( response );
        return true;
    }

    StateMachine& stateMachine = StateMachine::instance();

    Sptr<StateEvent> newEvent = new StateEvent( msg );
    Sptr<State> startState = session->state();
    Sptr<State> resultState = stateMachine.process( startState, newEvent );

    if( resultState == NEXT_STATE )
    {
        if (RtspConfiguration::instance().logStatistics)
        {
            myStatsMutex.lock();
            myStats.myTotalNonSuccessReqRecvd++;
            myStatsMutex.unlock();
        }

        cpLog( LOG_DEBUG, "Unknown RTSP method, reply 400" );
        Sptr<RtspResponse> response = new RtspResponse( msg, RTSP_400_STATUS );
        sendResponse( response );
        return true;
    }
    else if( resultState == FATAL_STATE )
    {
        cpLog( LOG_DEBUG, "Fatal state occured, shutdown server" );
        return false;
    }

    return true;
}


bool
RtspServer::processServerRequest( Sptr<RtspRequest> msg )
{
    Sptr< RtspTcpBuffer > tcpBuffer = msg->getTransConnPtr();
    assert( tcpBuffer != 0 );
    Connection conn = tcpBuffer->getConnection();

    if( conn.getIp() != networkAddress().getIpName() )
    {
        cpLog( LOG_ERR,
               "Rejecting server request because ip check failed" );
        return true;
    }

    u_int32_t mthd = msg->getMethod();
    switch( mthd )
    {
        case RTSP_TEARDOWN_MTHD:
        {
            cpLog( LOG_DEBUG, "Handling TEARDOWN, shutdown server" );
            if (RtspConfiguration::instance().logStatistics)
            {
                myStats.report();
            }
            // return false to break out of while() loop
            return false;
            break;
        }
        default:
            assert( 0 );
    }

    return true;
}


void
RtspServer::sendResponse( Sptr<RtspResponse> response )
{
    if (RtspConfiguration::instance().logStatistics)
    {
        myStatsMutex.lock();
        myStats.myTotalRespSent++;
        myStatsMutex.unlock();
    }

    myMsgInterface.send( response );
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
