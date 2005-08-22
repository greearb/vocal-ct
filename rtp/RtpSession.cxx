
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
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const RtpSession_cxx_Version =
    "$Id: RtpSession.cxx,v 1.9 2005/08/22 06:55:50 greear Exp $";


#include "global.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include "vtypes.h"
#include "cpLog.h"
#include "vsock.hxx"

#include "rtpTypes.h"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "RtpSession.hxx"


/* ----------------------------------------------------------------- */
/* --- rtp session Constructor ------------------------------------- */
/* ----------------------------------------------------------------- */

RtpSession::RtpSession (uint16 tos, uint32 priority, const string& local_ip,
                        const string& local_dev_to_bind_to,
                        const char* remoteHost, int remotePort, int localPort,
                        int rtcpRemotePort, int rtcpLocalPort, int portRange,
                        RtpPayloadType format, int clockrate,
                        int per_sample_size, int samplesize)
{
   constructRtpSession (tos, priority, local_ip, local_dev_to_bind_to,
                        remoteHost, remotePort, localPort, rtcpRemotePort,
                        rtcpLocalPort, portRange, format,
                        clockrate, per_sample_size, samplesize);
}

RtpSession::RtpSession (uint16 tos, uint32 priority, const string& local_ip,
                        const string& local_dev_to_bind_to,
                        const char* remoteHost, int remotePort, int localPort,
                        int rtcpRemotePort, int rtcpLocalPort,
                        RtpPayloadType format, int clockrate, int per_sample_size,
                        int samplesize)
{
   constructRtpSession (tos, priority, local_ip, local_dev_to_bind_to,
                        remoteHost, remotePort, localPort, rtcpRemotePort,
                        rtcpLocalPort, 0, format, clockrate, per_sample_size,
                        samplesize);
}

void RtpSession::constructRtpSession (uint16 tos, uint32 priority,
                                      const string& local_ip,
                                      const string& local_dev_to_bind_to,
                                      const char* remoteHost,
                                      int remotePort, int localPort,
                                      int rtcpRemotePort, int rtcpLocalPort,
                                      int portRange, RtpPayloadType format,
                                      int clockrate, int per_sample_size,
                                      int samplesize)
{

    cpLog(LOG_DEBUG, "Creating RTP session.");
    cpLog(LOG_DEBUG, "Listening on port: %d", localPort);
    cpLog(LOG_DEBUG, "Bound to device: %s", local_dev_to_bind_to.c_str());
    cpLog(LOG_DEBUG, "Sending to remote host: %s", remoteHost);
    cpLog(LOG_DEBUG, "Sending to remote port: %d", remotePort);

    recv = NULL; tran = NULL;
    rtcpTran = NULL, rtcpRecv = NULL;

    if (localPort != 0)
    {
        if (portRange != 0)
           recv = new RtpReceiver (tos, priority, local_ip, local_dev_to_bind_to,
                                   localPort, localPort + portRange,
                                   format, clockrate, per_sample_size, samplesize);
        else
           recv = new RtpReceiver (tos, priority, local_ip, local_dev_to_bind_to,
                                   localPort, format, clockrate, per_sample_size, samplesize);
    }

    if (remotePort != 0)
    {
        if (portRange != 0)
           tran = new RtpTransmitter (tos, priority, local_ip, local_dev_to_bind_to,
                                      remoteHost, remotePort,
                                      remotePort + portRange, format,
                                      clockrate, per_sample_size, samplesize, recv);
        else
           tran = new RtpTransmitter (tos, priority, local_ip, local_dev_to_bind_to,
                                      remoteHost, remotePort, format,
                                      clockrate, per_sample_size, samplesize, recv);
    }

    if (rtcpLocalPort != 0)
    {
        if (portRange != 0)
           rtcpRecv = new RtcpReceiver (tos, priority, local_ip, local_dev_to_bind_to,
                                        rtcpLocalPort,
                                        rtcpLocalPort + portRange);
        else
           rtcpRecv = new RtcpReceiver (tos, priority, local_ip, local_dev_to_bind_to,
                                        rtcpLocalPort);
    }

    if (rtcpRemotePort != 0) {
       if (portRange != 0)
          rtcpTran = new RtcpTransmitter (tos, priority, local_ip, local_dev_to_bind_to,
                                          remoteHost, rtcpRemotePort,
                                          rtcpRemotePort + portRange, rtcpRecv);
        else
           rtcpTran = new RtcpTransmitter(tos, priority, local_ip, local_dev_to_bind_to,
                                          remoteHost, rtcpRemotePort, rtcpRecv);
    }


    // update interlinks
    if (rtcpTran && tran) rtcpTran->setRTPtran (tran);
    if (rtcpTran && recv) rtcpTran->setRTPrecv (recv);
    if (rtcpTran && rtcpRecv) rtcpTran->setRTCPrecv (rtcpRecv);
    if (rtcpRecv && recv) recv->setRTCPrecv(rtcpRecv);


    // SDES infromation for transmitter
    if (rtcpTran && tran)
    {
        char dummy[2] = "";

        rtcpTran->setSdesCname();
        rtcpTran->setSdesName(dummy);
        rtcpTran->setSdesEmail(dummy);
        rtcpTran->setSdesPhone(dummy);
        rtcpTran->setSdesLoc(dummy);
        rtcpTran->setSdesTool(dummy);
        rtcpTran->setSdesNote(dummy);
    }


    // session states
    // Currently the states are for RTP stack only, not for RTCP
    if (tran && recv)
    {
        sessionState = rtp_session_sendrecv;
        recv->getUdpStack()->setMode(sendrecv);
    }
    else if (tran && !recv)
    {
        sessionState = rtp_session_sendonly;
        tran->getUdpStack()->setMode(sendonly);
    }
    else if (recv && !tran)
    {
        sessionState = rtp_session_recvonly;
        recv->getUdpStack()->setMode(recvonly);
    }
    else
    {
        cpLog (LOG_ERR, "Session undefined");
        sessionState = rtp_session_undefined;
    }


    if (tran) cpLog (LOG_DEBUG_STACK, "RTP Tran Port: %d",
                         tran->getUdpStack()->getDestinationPort());
    if (recv) cpLog (LOG_DEBUG_STACK, "RTP Recv Port: %d", recv->getPort());
    if (rtcpTran) cpLog (LOG_DEBUG_STACK, "RTCP Tran Port: %d",
                             rtcpTran->getUdpStack()->getDestinationPort());
    if (rtcpRecv) cpLog (LOG_DEBUG_STACK, "RTCP Recv Port: %d",
                             rtcpRecv->getPort());
}

RtpSession::~RtpSession ()
{
    if( rtcpTran )
    {
        delete rtcpTran;
        rtcpTran = NULL;
    }
    if( rtcpRecv )
    {
        delete rtcpRecv;
        rtcpRecv = NULL;
    }
    if( tran )
    {
        delete tran;
        tran = NULL;
    }
    if( recv )
    {
        delete recv;
        recv = NULL;
    }
}

int RtpSession::reserveRtpPort(uint16 tos, uint32 priority,
                               const string& local_ip,
                               const string& local_dev_to_bind_to,
                               int localMin, int localMax) {
   int port = 0;
   
   if ( recv == 0 ) {
      try {
         //let RtpReceiver() automatically generate a port number
         recv = new RtpReceiver(tos, priority, local_ip, local_dev_to_bind_to,
                                localMin, localMax,
                                rtpPayloadPCMU, 8000, 1, 160);
         port = recv->getPort();
      }
      catch ( UdpStackExceptionPortsInUse& ) {
         if ( localMin == localMax )
            cpLog( LOG_ERR, "port %d is not available", localMin );
         else
            cpLog( LOG_ERR, "No ports between %d and %d are available",
                   localMin, localMax);
         recv = 0;
      }
   }
   else {
      port = recv->getPort();
   }
   
   return port;
}

int RtpSession::releaseRtpPort() {
   int port = 0;
   if ( recv != 0 ) {
      port = recv->getPort();
      delete recv;
      recv = 0;
   }
   return port;
}

int
RtpSession::reserveRtcpPort(uint16 tos, uint32 priority,
                            const string& local_ip,
                            const string& local_dev_to_bind_to,
                            int rtcpLocalPort, int portRange) {
   int port = 0;

   if ( rtcpRecv == 0 ) {
      try {
         if (rtcpLocalPort != 0) {
            if (portRange != 0)
               rtcpRecv = new RtcpReceiver (tos, priority, local_ip, local_dev_to_bind_to,
                                            rtcpLocalPort,
                                            rtcpLocalPort + portRange);
            else
               rtcpRecv = new RtcpReceiver (tos, priority, local_ip, local_dev_to_bind_to,
                                            rtcpLocalPort);
            }
         
         port = rtcpRecv->getPort();
      }
      catch ( UdpStackExceptionPortsInUse& ) {
         if ( portRange == 0 )
            cpLog( LOG_ERR, "port %d is not available", rtcpLocalPort );
         else
            cpLog( LOG_ERR, "no ports between %d and %d are available",
                   rtcpLocalPort, rtcpLocalPort + portRange );
         rtcpRecv = 0;
      }
   }
   else {
      port = rtcpRecv->getPort();
   }
   
   return port;
}

int RtpSession::releaseRtcpPort() {
   int port = 0;
   if ( rtcpRecv != 0 ) {
      port = rtcpRecv->getPort();
      delete rtcpRecv;
      rtcpRecv = 0;
   }
   return port;
}

/* --- Send and Receive RTP Functions ------------------------------ */
void RtpSession::setFormat (RtpPayloadType type, int clockrate, int per_sample_size,
                            int samplesize) {
   if (tran) {
      tran->setFormat(type);
      tran->setClockRate(clockrate);
      tran->setPerSampleSize(per_sample_size);
      tran->setSampleSize(samplesize);
   }
   
   if (recv) {
      recv->setFormat(type);
      recv->setClockRate(clockrate);
      recv->setPerSampleSize(per_sample_size);
      recv->setSampleSize(samplesize);
   }
}


void RtpSession::setCodecString (const char* codecStringInput) {
   if (tran) tran->setCodecString (codecStringInput);
   if (recv) recv->setCodecString (codecStringInput);
}


void RtpSession::setSampleSize (int no_samples) {
   if (tran) tran->setSampleSize (no_samples);
   if (recv) recv->setSampleSize (no_samples);
}


int RtpSession::getSampleSize () {
   if (tran)
      return tran->getSampleSize();
   else
      return 0;
}

// If there are multiple dests, the return will be the total
// of all dests. - ?
int RtpSession::getPacketSent () {
   if (tran)
      return tran->getPacketSent ();
   else
      return 0;
}

// If there are multiple dests, the return will be the total
// of all dests. - ?
int RtpSession::getByteSent () {
   if (tran)
      return tran->getPayloadSent ();
   else
      return 0;
}

// If there are multiple srcs, the return will be the total
// of all srcs. - ?
int RtpSession::getPacketReceived () {
   if (recv)
      return recv->getPacketReceived ();
   else
      return 0;
}

// If there are multiple srcs, the return will be the total
// of all srcs. - ?
int RtpSession::getByteReceived () {
   if (recv)
      return recv->getPayloadReceived ();
   else
      return 0;
}

// If there are multiple srcs, this func returns the packect lost
// for all the srcs
int RtpSession::getPacketLost () {
   if (rtcpTran) {
      if (rtcpRecv) {
         int lost = 0;
         for (int i = 0; i < (rtcpRecv->getTranInfoCount()); i++) {
            lost += rtcpTran->calcLostCount(rtcpRecv->getTranInfoList(i));
         }
         return lost;
      }
   }
   return 0;
}

// If there are multiple srcs, this func returns the jitter
// calculation results accumulated by all srcs
int RtpSession::getJitter () {
   if (recv) {
      return (recv->getJitter() >> 4);
   }
   return 0;
}

// If there are multiple srcs, what this func returns
// is not clear - ?
int RtpSession::getLatency () {
   if (rtcpRecv) {
      return (rtcpRecv->getAvgOneWayDelay());
      //        return (rtcpRecv->getAvgRoundTripDelay());
   }
   
   return 0;
}

void RtpSession::setSessionState (RtpSessionState state)
{
    switch (state)
    {
        case (rtp_session_inactive):
           if (recv) recv->getUdpStack()->setMode(inactive);
           else if (tran) tran->getUdpStack()->setMode(inactive);
        sessionState = state;
        break;
        case (rtp_session_sendonly):
           if (recv) recv->getUdpStack()->setMode(sendonly);
           else if (tran) tran->getUdpStack()->setMode(sendonly);
        sessionState = state;
        break;
        case (rtp_session_recvonly):
           if (recv) recv->getUdpStack()->setMode(recvonly);
           else if (tran) tran->getUdpStack()->setMode(recvonly);
        sessionState = state;
        break;
        case (rtp_session_sendrecv):
           if (recv) recv->getUdpStack()->setMode(sendrecv);
           else if (tran) tran->getUdpStack()->setMode(sendrecv);
        sessionState = state;
        break;
        case (rtp_session_undefined):
                    sessionState = state;
        break;
        default:
        cpLog (LOG_ERR, "Unknown state: %d", (int)state);
        assert (0);
    }

    if (recv) recv->emptyNetwork();
}


RtpSessionState RtpSession::getSessionState () {
   return sessionState;
}

int
RtpSession::setReceiver (uint16 tos, uint32 priority,
                         const string& local_ip,
                         const string& local_dev_to_bind_to,
                         int localPort, int rtcpLocalPort, int portRange,
                         RtpPayloadType format, int clockrate, int per_sample_size,
                         int samplesize) {
   if ( !(sessionState == rtp_session_sendrecv
          || sessionState == rtp_session_recvonly) ) {
      cpLog(LOG_ERR, "wrong state of RTP stack.");
      return -1;
   }

   if (recv) {
      cpLog(LOG_ERR, "WARNING:  Deleting recv in RtpSession::setReceiver..\n");
      delete recv;
      recv = NULL;
   }

   if (localPort != 0) {
      if (portRange != 0) {
         if (tran) {
            recv = new RtpReceiver(tran->getUdpStack(), format, clockrate,
                                   per_sample_size, samplesize);
         }
         else {
            recv = new RtpReceiver (tos, priority, local_ip, local_dev_to_bind_to,
                                    localPort, localPort + portRange,
                                    format, clockrate, per_sample_size,
                                    samplesize);
         }
      }
      else {
         if (tran) {
            recv = new RtpReceiver(tran->getUdpStack(), format, clockrate,
                                   per_sample_size, samplesize);
         }
         else {
            recv = new RtpReceiver (tos, priority, local_ip, local_dev_to_bind_to,
                                    localPort, format, clockrate, per_sample_size,
                                    samplesize);
         }
      }
   }
   
   if (rtcpRecv) {
      cpLog(LOG_ERR, "WARNING:  Deleting rtcpRecv in RtpSession::setReceiver..\n");
      delete rtcpRecv;
      rtcpRecv = NULL;
   }
      

   if (rtcpLocalPort != 0) {
      if (portRange != 0) {
         if (rtcpTran) {
            rtcpRecv = new RtcpReceiver(rtcpTran->getUdpStack());
         }
         else {
            rtcpRecv = new RtcpReceiver (tos, priority, local_ip, local_dev_to_bind_to,
                                         rtcpLocalPort,
                                         rtcpLocalPort + portRange);
         }
      }
      else {
         if (rtcpTran) {
            rtcpRecv = new RtcpReceiver(rtcpTran->getUdpStack());
         }
         else {
            rtcpRecv = new RtcpReceiver (tos, priority, local_ip, local_dev_to_bind_to,
                                         rtcpLocalPort);
         }
      }
   }
   
   // update interlinks
   if (rtcpTran && recv) rtcpTran->setRTPrecv (recv);
   if (rtcpTran && rtcpRecv) rtcpTran->setRTCPrecv (rtcpRecv);
   if (rtcpRecv && recv) recv->setRTCPrecv(rtcpRecv);

   if (recv)
      cpLog (LOG_DEBUG_STACK, "RTP Recv Port: %d", recv->getPort());
   if (rtcpRecv)
      cpLog (LOG_DEBUG_STACK, "RTCP Recv Port: %d",
             rtcpRecv->getPort());

   return 0;
}


int
RtpSession::setTransmiter(uint16 tos, uint32 priority,
                          const string& local_ip,
                          const string& local_dev_to_bind_to,
                          const char* remoteHost, int remotePort,
                          int rtcpRemotePort,
                          RtpPayloadType format, int clockrate, int per_sample_size,
                          int samplesize) {
   if ( !(sessionState == rtp_session_sendrecv
          || sessionState == rtp_session_sendonly) ) {
      cpLog(LOG_ERR, "wrong state of RTP stack.");
      return -1;
   }

   if (remotePort != 0) {
      if (tran) {
         assert(remoteHost);
         tran->getUdpStack()->setDestination(remoteHost, remotePort);
         NetworkAddress netAddress(remoteHost, remotePort);
         tran->setRemoteAddr(netAddress);
      }
      else {
         tran = new RtpTransmitter (tos, priority, local_ip, local_dev_to_bind_to,
                                    remoteHost, remotePort, format,
                                    clockrate, per_sample_size, samplesize,
                                    recv);
      }
   }
   
   if (rtcpRemotePort != 0) {
      if (rtcpTran) {
         assert(remoteHost);
         rtcpTran->getUdpStack()->setDestination( remoteHost, rtcpRemotePort);
         NetworkAddress netAddress1(remoteHost, rtcpRemotePort);
         rtcpTran->setRemoteAddr(netAddress1);
      }
      else {
         rtcpTran = new RtcpTransmitter(tos, priority, local_ip, local_dev_to_bind_to,
                                        remoteHost, rtcpRemotePort, rtcpRecv);
      }
   }
   
   // update interlinks
   if (rtcpTran && tran) rtcpTran->setRTPtran (tran);
   if (rtcpTran && recv) rtcpTran->setRTPrecv (recv);
   if (rtcpTran && rtcpRecv) rtcpTran->setRTCPrecv (rtcpRecv);

   // SDES infromation for transmitter
   if (rtcpTran && tran) {
      char dummy[2]= "";
      
      rtcpTran->setSdesCname();
      rtcpTran->setSdesEmail(dummy);
      rtcpTran->setSdesPhone(dummy);
      rtcpTran->setSdesLoc(dummy);
      rtcpTran->setSdesTool(dummy);
      rtcpTran->setSdesNote(dummy);
   }

   if (tran) cpLog (LOG_DEBUG_STACK, "RTP Tran Port: %d",
                    tran->getUdpStack()->getDestinationPort());
   if (rtcpTran) cpLog (LOG_DEBUG_STACK, "RTCP Tran Port: %d",
                        rtcpTran->getUdpStack()->getDestinationPort());
   
   return 0;
}


/* --- Send and Receive RTP Functions ------------------------------ */

RtpPacket* RtpSession::createPacket (int npadSize, int csrcCount)
{
    assert (tran);
    if (npadSize != 0)
        cpLog(LOG_DEBUG, "Nonzero npadSize not supported");
    return tran->createPacket(npadSize, csrcCount);
}



RtpSeqNumber RtpSession::getPrevSequence ()
{
    assert (tran);
    return tran->getPrevSequence ();
}



RtpTime RtpSession::getPrevRtpTime ()
{
    assert (tran);
    return tran->getPrevRtpTime();
}



void RtpSession::setMarkerOnce()
{
    if( tran )
    {
        tran->setMarkerOnce();
    }
}


int RtpSession::transmit (RtpPacket& pkt)
{
    if ( !( sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_sendonly ) )
    {
        sessionError = session_wrongState;
        cpLog (LOG_ERR, "RTP stack can't transmit. Wrong state");
        return -1;
    }

    assert (tran);
    sessionError = session_success;
    return tran->transmit(pkt);
}

/** Let the stack know we are suppressing an RTP packet send
 * due to VAD...for accounting purposes mainly.
 */
int RtpSession::notifyVADSuppression(int len) {
   static int pkt_sent = 0;
   
   if ( !( sessionState == rtp_session_sendrecv
           || sessionState == rtp_session_sendonly ) ) {
      sessionError = session_wrongState;
      cpLog (LOG_ERR, "RTP stack can't transmit. Wrong state");
      return -1;
   }

   // Check to see if we should send an RTCP packet...
   // Only check every 8 packets or so, for efficiency. --Ben
   if ((pkt_sent++ & 0x7) == 0) {
      if (rtcpTran) {
         if (checkIntervalRTCP()) {
            transmitRTCP();
         }
      }
   }
   
   assert (tran);
   sessionError = session_success;
   return tran->notifyVADSuppression(len);
}


int RtpSession::transmitRaw (char* inbuffer, int len) {
   static int pkt_sent = 0;
   
   if ( !( sessionState == rtp_session_sendrecv
           || sessionState == rtp_session_sendonly ) ) {
      sessionError = session_wrongState;
      cpLog (LOG_ERR, "RTP stack can't transmit. Wrong state");
      return -1;
   }

   // Check to see if we should send an RTCP packet...
   // Only check every 8 packets or so, for efficiency. --Ben
   if ((pkt_sent++ & 0x7) == 0) {
      if (rtcpTran) {
         if (checkIntervalRTCP()) {
            transmitRTCP();
         }
      }
   }
   
   assert (tran);
   sessionError = session_success;
   return tran->transmitRaw(inbuffer, len);
}


int RtpSession::transmitEvent( int event )
{
    if ( !( sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_sendonly ) )
    {
        sessionError = session_wrongState;
        cpLog (LOG_ERR, "RTP stack can't transmit event. Wrong state");
        return -1;
    }

    cpLog( LOG_DEBUG_STACK,"Sending DTMF event %d in RTP stream", event );
    assert (tran);
    sessionError = session_success;
    RtpPacket* eventPacket = createPacket(0, 0);
    eventPacket->setPayloadType( rtpPayloadDTMF_RFC2833 );
    eventPacket->setPayloadUsage( sizeof( RtpEventDTMFRFC2833 ) );
    RtpEventDTMFRFC2833* eventPayload = reinterpret_cast<RtpEventDTMFRFC2833*>
                                        ( eventPacket->getPayloadLoc() );

    // reset event payload
    eventPayload->event = event; 
    eventPayload->volume = 0x0A;
    eventPayload->reserved = 0;
    eventPayload->edge = 0;
    eventPayload->duration = 0x00A0;

    // send onedge packet
    tran->transmit( *eventPacket, true );

    // send on packet
    eventPayload->edge = 1;
    tran->transmit( *eventPacket, true );

    // send offedge packet
    eventPacket->setPayloadUsage( 0 );
    tran->transmit( *eventPacket, true );

    if ( eventPacket ) {
        delete eventPacket;
        eventPacket = NULL;
    }
    return 0;
}


int RtpSession::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                       int& maxdesc, uint64& timeout, uint64 now) {
   if (recv) {
      recv->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   }
   if (rtcpRecv) {
      rtcpRecv->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   }
   return 0;
}


int RtpSession::retrieve(RtpPacket& pkt, const char* dbg) {
   return recv->retrieve(pkt, dbg);
}

int RtpSession::readNetwork(fd_set* fds) {
   if ( !( sessionState == rtp_session_sendrecv
           || sessionState == rtp_session_recvonly ) ) {
      if (recv) {
         if (fds && FD_ISSET(recv->getUdpStack()->getSocketFD(), fds)) {
            recv->readNetwork();
         }
         sessionError = session_wrongState;
         cpLog (LOG_ERR, "RTP stack can't receive. Wrong state");
      }
      return -EINVAL;
   }

   if (rtcpRecv) {
      if (!fds || (FD_ISSET(rtcpRecv->getUdpStack()->getSocketFD(), fds))) {
         receiveRTCP();
      }
   }
   
   sessionError = session_success;
   if (fds && FD_ISSET(recv->getUdpStack()->getSocketFD(), fds)) {
      recv->readNetwork();
   }
   return 0;
}//readNetwork


/* --- Send and Receive RTCP Functions ----------------------------- */

void RtpSession::processRTCP ()
{
   if (rtcpTran) {
      if (checkIntervalRTCP()) {
         transmitRTCP();
      }
   }

   if (rtcpRecv) {
      receiveRTCP();
   }

   return ;
}


int RtpSession::transmitRTCP ()
{
    if ( !( sessionState == rtp_session_sendrecv
            || sessionState == rtp_session_sendonly ) )
    {
        sessionError = session_wrongState;
        cpLog (LOG_ERR, "RTCP stack can't transmit. Wrong state");
        return -1;
    }

    assert (rtcpTran);

    // generate compound packet
    RtcpPacket p;

    // load with report packet
    rtcpTran->addSR(&p);

    // load with SDES information
    // currently only sender sends SDES, recv only receiver doesn't
    if (tran)
       rtcpTran->addSDES(&p);

    // transmit packet
    int ret = rtcpTran->transmit(p);

    return ret;
}



int RtpSession::transmitRTCPBYE ()
{
    assert (rtcpTran);

    // generate compound packet
    RtcpPacket p;

    // load with report packet
    rtcpTran->addSR(&p);

    // load with SDES CNAME
    if (tran)
       rtcpTran->addSDES(&p, rtcpSdesCname);

    // load with BYE packet
    if (tran)
    {
        char reason[] = "Program Ended.";
        rtcpTran->addBYE(&p, reason);
    }

    // transmit packet
    int ret = rtcpTran->transmit(p);

    return ret;
}


int RtpSession::receiveRTCP ()
{
   RtcpPacket pkt;
   int rv = -1;
   if ( !( sessionState == rtp_session_sendrecv
           || sessionState == rtp_session_recvonly ) ) {
      if (rtcpRecv->getPacket(pkt) > 0) {
         sessionError = session_wrongState;
         cpLog (LOG_ERR, "RTCP stack can't receive. Wrong state");
      }
      return -1;
   }

   // Only read one pkt at once.
   if (rtcpRecv->getPacket(pkt) > 0) {
      // read compound packet
      if (rtcpRecv->readRTCP(&pkt) == 1) {
         rv = 1;
      }
      else {
         rv = 2; // TODO:  Figure out who (if anyone) pays attention to these return codes
      }
   }

   return rv;
}


int RtpSession::checkIntervalRTCP ()
{
    assert (rtcpTran);
    return rtcpTran->checkInterval();
}




/* --- DTMF Callback functions ------------------------------------- */
void RtpSession::setDTMFInterface( DTMFInterface* t )
{
    if ( !recv )
        cpLog(LOG_ERR, "RTP receiver not set, can't set DTMF Interface");
    else
        recv->setDTMFInterface( t );
}

void RtpSession::unsetDTMFInterface( DTMFInterface* t )
{
    if ( !recv )
        cpLog(LOG_ERR, "RTP receiver not set, can't unset DTMF Interface");
    else
        recv->unsetDTMFInterface( t );
}
