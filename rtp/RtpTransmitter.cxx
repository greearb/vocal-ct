/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000,2003 Vovida Networks, Inc.  All rights reserved.
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

#include "global.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include "vtypes.h"
#include <unistd.h>
#include <string.h>


// network socket
//#include <netinet/in.h>                // struct socketaddr_in
//#include <sys/socket.h>
//#include <netdb.h>

#include "cpLog.h"
#include "vsock.hxx"
#include "NetworkAddress.h"

#include "NtpTime.hxx"
#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "Rtp.hxx"
#include "rtpCodec.h"


/* ----------------------------------------------------------------- */
/* --- RtpTransmitter Constructor ---------------------------------- */
/* ----------------------------------------------------------------- */

RtpTransmitter::RtpTransmitter (uint16 tos, uint32 priority,
                                const string& local_ip,
                                const string& local_dev_to_bind_to,
                                const char* remoteHost,
                                int remoteMinPort, int remoteMaxPort,
                                RtpPayloadType _format, int clockrate,
                                int per_sample_size, int samplesize,
                                RtpReceiver* receiver)
      :
      remoteAddr(remoteHost, remoteMinPort),
      rtp_raw_tx_pkt(1012)
{
    assert(remoteHost);

    // TODO:  What if the remote is not on the minimum port?  How do we
    // TODO:  deal with that???

    if(  receiver ) {
       myStack = receiver->getUdpStack();
       myStack->setDestination(&remoteAddr);
    }
    else {
       myStack = new UdpStack (tos, priority, false, local_ip, local_dev_to_bind_to,
                               &remoteAddr, remoteMinPort,
                               remoteMaxPort, sendonly) ;
    }
    constructRtpTransmitter (_format, clockrate, per_sample_size, samplesize);
}


RtpTransmitter::RtpTransmitter (uint16 tos, uint32 priority,
                                const string& local_ip,
                                const string& local_dev_to_bind_to,
                                const char* remoteHost, int remotePort,
                                RtpPayloadType _format, int clockrate,
                                int per_sample_size, int samplesize,
                                RtpReceiver* receiver)
      :
      remoteAddr(remoteHost, remotePort),
      rtp_raw_tx_pkt(1012)
{
   assert(remoteHost);

   if(  receiver ) {
      myStack = receiver->getUdpStack();
      myStack->setDestination(&remoteAddr);
   }
   else {
      myStack = new UdpStack (tos, priority, false, local_ip, local_dev_to_bind_to,
                              &remoteAddr, remotePort,
                              remotePort, sendonly) ;
   }
   constructRtpTransmitter (_format, clockrate, per_sample_size, samplesize);
}


void RtpTransmitter::constructRtpTransmitter (RtpPayloadType _format, int clockrate,
                                              int per_sample_size, int samplesize) {
   // set format and baseSampleRate
   format = _format;
   clockRate = clockrate;
   perSampleSize = per_sample_size;
   sampleSize = samplesize;

   // set private variables
   ssrc = generateSRC();
   seedNtpTime = getNtpTime();
   seedRtpTime = generate32();
   prevNtpTime = seedNtpTime;
   prevRtpTime = seedRtpTime;
   prevSequence = generate32();

   // set counters
   packetSent = 0;
   payloadSent = 0;
   codecString[0] = '\0';

   cpLog(LOG_DEBUG_STACK, "Constructed ssrc = %d", ssrc);
}


RtpTransmitter::~RtpTransmitter () {
    cpLog(LOG_DEBUG_STACK, "Closed ssrc = %d", ssrc);
}

void RtpTransmitter::setRemoteAddr (const NetworkAddress& theAddr) {
   remoteAddr = theAddr;
   myStack->setDestination(&remoteAddr);
   cpLog(LOG_WARNING, "RtpTransmitter: %p  setting Remote Addr: %s\n", this, theAddr.toString().c_str());
}

/* --- send packet functions --------------------------------------- */

RtpPacket* RtpTransmitter::createPacket (int npadSize, int csrc_count)
{
    // create packet
    RtpPacket* packet = new RtpPacket (1024, npadSize, csrc_count);
    assert (packet);

    // load packet
    packet->setSSRC (ssrc);
    packet->setPayloadType (format);

    return packet;
}


// takes api RTP packet and send to network
// assumes payload size is already set
int RtpTransmitter::transmit(RtpPacket& pkt, bool eventFlag ) {
   if (!pkt.convertedToNBO()) {
      rtp_htonl(&pkt);
   }

   // finish packet

   if( !pkt.isTimestampSet() )
      pkt.setRtpTime( prevRtpTime + sampleSize );
   if( !pkt.isSequenceSet() )
      pkt.setSequence( prevSequence + 1 );
   if (pkt.getPayloadUsage() < 0) {
      cpLog(LOG_ERR, "ERROR: Invalid data packet size %d", pkt.getPayloadUsage());
      return -1;
   }

   //set marker once
   if( markerOnce ) {
      cpLog( LOG_DEBUG_STACK,"Setting marker bit once");
      pkt.setMarkerFlag(1);
      markerOnce = false;
   }

   // for packet reuse
   pkt.setTimestampSet(false);
   pkt.setSequenceSet(false);
   
   // transmit packet
   myStack->queueTransmitTo( (char*)pkt.getHeader(), pkt.getTotalUsage(), &remoteAddr);
   
   // update counters
   packetSent++;

#ifdef USE_LANFORGE
   uint64 now;
#endif

   prevSequence = pkt.getSequence();
   if( !eventFlag ) {
      payloadSent += pkt.getPayloadUsage();
      prevNtpTime = getNtpTime();
      prevRtpTime = pkt.getRtpTime();

#ifdef USE_LANFORGE
      // Save a system call, since we just grabbed the timestamp anyway.
      now = prevNtpTime.getMs();
#endif
   }
   else {
#ifdef USE_LANFORGE
      now = vgetCurMs();
#endif
   }

   // set up return value
   int result = pkt.getPayloadUsage();

#ifdef USE_LANFORGE
   if (rtpStatsCallbacks) {
      rtpStatsCallbacks->avgNewRtpTx(now, 1, result);
   }
#endif

   // exit with success
   return result;
}


// takes rawdata, pokes it in an RTP packet, and sends it.
// It does no additional buffering, the data should be exactly
// enough for one RTP payload.  Cannot otherwise work with variable-sized
// codecs. --Ben
int RtpTransmitter::transmitRaw (const char* data, int len) {
   assert (data);
   assert(len >= 0);
   
   // Initialize our packet.
   rtp_raw_tx_pkt.clear();
   rtp_raw_tx_pkt.recalcPacketAlloc(len);
   
   rtp_raw_tx_pkt.setSSRC(ssrc);
   rtp_raw_tx_pkt.setPayloadType(format);
   rtp_raw_tx_pkt.setPayloadUsage(len);
   
   memcpy (rtp_raw_tx_pkt.getPayloadLoc(), data, len);
   
   // finish packet
   return transmit(rtp_raw_tx_pkt);
}

/** Let the stack know we are suppressing an RTP packet send
 * due to VAD...for accounting purposes mainly.
 */
int RtpTransmitter::notifyVADSuppression(int len) {
#ifdef USE_LANFORGE
    if (rtpStatsCallbacks) {
       uint64 now = vgetCurMs();
       rtpStatsCallbacks->avgNewRtpTxVAD(now, 1, len);
    }
#endif
    return 0;
}

void RtpTransmitter::setFormat (RtpPayloadType newtype) {
    format = newtype;
}

void RtpTransmitter::setCodecString(const char* codecStringInput)
{
    strncpy(codecString, codecStringInput, 32);
}

