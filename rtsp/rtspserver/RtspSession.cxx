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

static const char* const RtspSession_cxx_version =
    "$Id: RtspSession.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspSession.hxx"
#include "VException.hxx"


RtspSession::RtspSession()
    : myCurrentNpt( -1 ),
      myPausePoint( -1 ),
      myPacketTotal( -1 ),
      myPacketCounter( -1 ),
      myPacketIntervalMs( -1 ),
      myFileHandler( 0 ),
      myRtpSession( 0 ),
      mySessionMode( RTSP_SESSION_MODE_UNKNOWN ),
      myRtpFd( -1 )
{
    cpLog( LOG_DEBUG, "Creating new RtspSession" );
}


RtspSession::~RtspSession()
{
    deleteRtpStack();
}


void
RtspSession::addPendingEvent( Sptr<RtspPlayMsg> event )
{
    myPendingEvents.add( event );
}



bool
RtspSession::createRtpStack( int mode )
{
    if( fileHandler() == 0 )
    {
        cpLog( LOG_ERR, "FileHandler not set" );
        assert( 0 );
    }

    if( mode == 0 )
    {
        cpLog( LOG_DEBUG, "Open new rtp session with mode PLAY" );
        printRtpInfo();
        RtpSession* newRtpSession = 0;
        try
        {
            newRtpSession = new RtpSession(
                                myClientAddr.getIpName().c_str(),
                                myTransportSpec.myClientPortA,
                                0,
                                0, 0 );
        }
        catch( UdpStackException& e )
        {
            cpLog( LOG_ALERT, "Fail in create newRtpSession: %s",
                   e.getDescription().c_str() );
            return false;
        }
        if( !newRtpSession )
        {
            cpLog( LOG_ERR, "Fail to create newRtpSession" );
            return false;
        }

        myRtpSession = newRtpSession;
        mySessionMode = RTSP_SESSION_MODE_PLAY;
    }
    else if( mode == 1 )
    {
        cpLog( LOG_DEBUG, "Open new rtp session with mode RECORD" );
        printRtpInfo();
        RtpSession* newRtpSession = 0;
        try
        {
            newRtpSession = new RtpSession(
                                0,
                                0,
                                myTransportSpec.myServerPortA,
                                0, 0 );
        }
        catch( UdpStackException& e )
        {
            cpLog( LOG_ALERT, "Fail in create newRtpSession: %s",
                   e.getDescription().c_str() );
            return false;
        }
        if( !newRtpSession )
        {
            cpLog( LOG_ERR, "Fail to create newRtpSession" );
            return false;
        }

        myRtpSession = newRtpSession;
        mySessionMode = RTSP_SESSION_MODE_REC;

        // save socket file descriptor for recv rtp processor
        rtpFd( myRtpSession->getRtpRecv()->getUdpStack()->getSocketFD() );
    }
    else
    {
        cpLog( LOG_ERR, "Invalid mode(%d) while opening rtp stack", mode );
        return false;
    }

    RtpPayloadType fileCodec = (RtpPayloadType)fileHandler()->packetCodec();
    RtpPayloadType dynCodec = (RtpPayloadType)fileHandler()->dynamicCodec();
    if (mode == 0)
    {
        if ((dynCodec > 0) &&
            ((fileCodec == rtpPayloadG726_16) ||
             (fileCodec == rtpPayloadG726_24) ||
             (fileCodec == rtpPayloadGSMEFR)))
        {
            myRtpSession->setApiFormat(dynCodec,
                                       fileHandler()->packetSamples(),
                                       fileHandler()->packetSize());
            myRtpSession->setNetworkFormat( dynCodec,
                                       fileHandler()->packetSamples(),
                                       fileHandler()->packetSize());
        }
        else
        {
            myRtpSession->setApiFormat(fileCodec,
                                       fileHandler()->packetSamples(),
                                       fileHandler()->packetSize() );
            myRtpSession->setNetworkFormat(fileCodec,
                                           fileHandler()->packetSamples(),
                                           fileHandler()->packetSize() );
        }
    }
    else
    {
        myRtpSession->setApiFormat(fileCodec,
                                   fileHandler()->packetSamples(),
                                   fileHandler()->packetSize() );
        myRtpSession->setNetworkFormat(fileCodec,
                                       fileHandler()->packetSamples(),
                                       fileHandler()->packetSize() );

        if ((fileCodec == rtpPayloadG726_16) ||
            (fileCodec == rtpPayloadG726_24) ||
            (fileCodec == rtpPayloadGSMEFR))
        {
            myRtpSession->setCodecString(
                rtpFileTypeInfo[fileHandler()->ftIndex()].name.c_str());
        }
    }

    return true;
}


void
RtspSession::deleteRtpStack()
{
    if( !myRtpSession )
    {
        return;
    }

    cpLog( LOG_DEBUG, "Deleting rtp session" );
    delete myRtpSession;
    myRtpSession = 0;
}


void
RtspSession::printRtpInfo()
{
    cpLog( LOG_DEBUG, "Client: %s %d %d", myClientAddr.getIpName().c_str(),
                               myTransportSpec.myClientPortA,
                               myTransportSpec.myClientPortB );
    cpLog( LOG_DEBUG, "Server: %s %d %d", myServerAddr.getIpName().c_str(),
                               myTransportSpec.myServerPortA,
                               myTransportSpec.myServerPortB );
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
