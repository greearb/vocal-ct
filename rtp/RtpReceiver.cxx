
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

static const char* const RtpReceiver_cxx_Version =
    "$Id: RtpReceiver.cxx,v 1.4 2004/06/22 02:24:04 greear Exp $";


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

#include "cpLog.h"
#include "vsock.hxx"

#include "NetworkAddress.h"
#include "NtpTime.hxx"
#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "rtpCodec.h"
#include <sstream>


string RtpData::toString() const {
   ostringstream oss;
   oss << " len: " << len << " is_in_use: " << is_in_use
       << " logically_empty: " << is_logically_empty
       << " silence_fill: " << is_silence_fill;
   return oss.str();
}

static NtpTime nowTime, pastTime;
/* ----------------------------------------------------------------- */
/* --- RtpReceiver Constructor ------------------------------------- */
/* ----------------------------------------------------------------- */

RtpReceiver::RtpReceiver (const string& local_ip,
                          const string& local_dev_to_bind_to,
                          int localMinPort, int localMaxPort,
                          RtpPayloadType _format, int _clockrate,
                          int per_sample_size, int samplesize) {
    /// udp stack is a sendrecv stack
    myStack = new UdpStack (false, local_ip, local_dev_to_bind_to,
                            NULL, localMinPort, localMaxPort) ;

    constructRtpReceiver (_format, _clockrate, per_sample_size,
                          samplesize);
}

RtpReceiver::RtpReceiver (const string& local_ip,
                          const string& local_dev_to_bind_to,
                          int localPort, RtpPayloadType _format,
                          int _clockrate, int per_sample_size,
                          int samplesize) {
    /// udp stack is a sendrecv stack
    myStack = new UdpStack (false, local_ip, local_dev_to_bind_to, NULL, localPort) ;

    constructRtpReceiver (_format, _clockrate, per_sample_size, samplesize);
}

RtpReceiver::RtpReceiver (Sptr<UdpStack> udp, RtpPayloadType _format,
                          int _clockrate, int per_sample_size,
                          int samplesize) {
    /// udp stack is a sendrecv stack
    myStack = udp;
    
    constructRtpReceiver (_format, _clockrate, per_sample_size, samplesize);
}


void RtpReceiver::constructRtpReceiver (RtpPayloadType _format,
                                        int _clockrate, int per_sample_size,
                                        int samplesize) {
    invalidPldSize = sidPktsRcvd = 0;
    inPos = 0;
    playPos = 0;
    cur_max_jbs = 16; // Jitter buffer size (in units of packets)

    for (int i = 0; i<JITTER_BUFFER_MAX; i++) {
       jitterBuffer[i] = NULL;
    }
    
    // Init our stats counters.
    recvCycles = playCycles = 0;
    silencePatched = insertedOooPkt = comfortNoiseDropped = 0;
    packetBadlyMisordered = packetTooOld = jitterBufferEmpty = 0;


    // set format and baseSampleRate
    setFormat(_format);

    clockRate = _clockrate;
    perSampleSize = per_sample_size;
    sampleSize = samplesize;

    cpLog(LOG_DEBUG_STACK, "sampleSize: %d  clockRate: %d  perSampleSize: %d  format: %d",
          sampleSize, clockRate, perSampleSize, format);

    // TODO:  Maybe can relax this some day, for large MTU networks..
    assert(sampleSize < 1472);

    // Was PROB-MAX and was static const.
    seq_loss_max = 100;

    // no transmitter
    sourceSet = false;
    ssrc = 0;
    probationSet = false;
    srcProbation = 0;
    probation = -2;
    prevPacket = NULL;
    rtcpRecv = NULL;
    silenceCodec = 0;
    codecString[0] = '\0';
    prevSeqRecv = 0;
    prevSeqPlay = 0;

    realPrevSeqRecv = 0;

    cpLog(LOG_DEBUG_STACK, "Constructed receiver");
}


RtpReceiver::~RtpReceiver () {
   rtcpRecv = NULL;

   clearBuffer();

   cpLog (LOG_DEBUG_STACK, "Close receiver");
}



int RtpReceiver::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                        int& maxdesc, uint64& timeout, uint64 now) {
   myStack->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   return 0;
}


/* --- receive packet functions ------------------------------------ */

int RtpReceiver::receive (RtpPacket& pkt,  fd_set* fds) {
    int len = 0;
    bool faking = 0;
    int rv = 0;
    int seq;

    bool read_ntwk = false;
    if (fds && FD_ISSET(myStack->getSocketFD(), fds)) {
       read_ntwk = true;
    }

    // empty network que
    NtpTime arrival (0, 0);
    while (read_ntwk) { // read all we can from the network.
        rv = getPacket(pkt);
        if (rv <= 0) {
           break;
        }

        // only play packets for valid sources
        if (probation < 0) {
            cpLog(LOG_ERR, "****Packet from invalid source");
            pkt.clear();
            continue;
        }

        arrival = getNtpTime();
        int packetTransit = 0;
        int delay = 0;

        rtp_ntohl(&pkt);

#if 0
        // This looks pretty bogus, and why wouldn't you need to do it on
        // sparc too?  If the codec changes mid-stream, then the user can
        // just hear/see garbage. --Ben

        // convert codec
        RtpPayloadType lType = pkt.getPayloadType();
        if ((lType != rtpPayloadDTMF_RFC2833) &&
            (lType != rtpPayloadCiscoRtp) &&
             lType != format)
        {
#ifndef __sparc
            // replace p with a new packet
            convertRtpPacketCodec (apiFormat, pkt);
#endif
        }
#endif

        len = pkt.getPayloadUsage();
        if (len <= 0 || len > 1012)
        {
            cpLog(LOG_ERR, "Got an invalid packet size");
            invalidPldSize++;
            pkt.clear();
            continue;
        }

#if 0
        // Don't know what this is, and don't like how it is being detected
        // (What about variable compression codecs??)
	/**/
        // drop SID packets
        if (!isSpeexFormat()) {
           if (len < XXXX) {
              cpLog( LOG_DEBUG_STACK, "Dropping SID packet" );
              setPrevSeqRecv(pkt.getSequence());
              sidPktsRcvd++;
              pkt.clear();
              continue;
           }
        }
	/**/
#endif

        // This does NOT deal with variable sized compression schemes. --Ben
        // fix frame boundry
        //if (len > networkFormat_payloadSize ) {
        //    int lenold = len;
        //    len = ( len / networkFormat_payloadSize ) * networkFormat_payloadSize;
        //    pkt.setPayloadUsage( len );
        //    cpLog( LOG_ERR, "Fixing frame boundry to %d from %d", len, lenold );
        //    sampleSize = (lenold / networkFormat_payloadSize) * sampleSize;
        //}

        // reordering the packets according to the seq no
        // leave gaps and copy the next packet in all the gap
        // when the late packets come copy it into the correct pos

        seq = pkt.getSequence();
        if (RtpSeqGreater(seq, prevSeqRecv)) {
           cpLog(LOG_DEBUG_STACK, "seq-greater:  pkt.seq: %d  prevSeq: %d, inPos: %d"
                 "  playPos: %d  cur_max_jbs: %d, queue_count: %d, sampleSize: %d",
                 seq, prevSeqRecv, inPos, playPos, cur_max_jbs,
                 getJitterPktsInQueueCount(), sampleSize);
           
           // C.Cameron, hacked by Ben Greear
           while (RtpTimeGreater( pkt.getRtpTime() - sampleSize,
                                  prevPacketRtpTime )) {
              
              // silence patching
              cpLog( LOG_ERR, "WARNING:  silence-patching, [(%d - %d > %d)], pkt.seq: %d prevSeq: %d", 
                     pkt.getRtpTime(), sampleSize, prevPacketRtpTime,
                     seq, prevSeqRecv);

              int tmp;
              assert((tmp = RtpSeqDifference(seq, prevSeqRecv)) > 1);

              if ( cpLogGetPriority() >= LOG_DEBUG_HB ) {
                 cerr << "s" << sampleSize;
              }

              if (jitterBuffer[inPos] == NULL) {
                 jitterBuffer[inPos] = new RtpData();
              }

              if (! isSpeexFormat()) {
                 if ( silenceCodec == 0 ) {
                    cpLog( LOG_DEBUG_STACK, "Patching silence" );
                    if ((pkt.getPayloadType() >= rtpPayloadDynMin) &&
                        (pkt.getPayloadType() <= rtpPayloadDynMax) &&
                        (codecString[0] != '\0')) {
                       silenceCodec = findSilenceCodecString(codecString, len);
                    }
                    else {
                       silenceCodec = findSilenceCodec( pkt.getPayloadType(), len );
                    }
                    
                    if ( silenceCodec == 0 ) {
                       if ( len > rtpCodecInfo[ numRtpCodecInfo - 1 ].length ) {
                          cpLog( LOG_ERR, "Requested codec too big to fake %d", len );
                          assert( 0 );
                       }
                       cpLog( LOG_DEBUG_STACK, "Faking silence packet with 0x00" );
                       silenceCodec = (char*)&rtpCodecInfo[ numRtpCodecInfo - 1 ].silence;
                       faking = true;
                    }
                 }
                 assert( silenceCodec );
                 
                 jitterBuffer[inPos]->setData(silenceCodec, len);
                 jitterBuffer[inPos]->setSilenceFill(true);
              }
              else {
                 // For speex, we let the codec deal with it, will flag on
                 // the 'isLogicallyEmpty' field in the RtpData object.
                 jitterBuffer[inPos]->setSilenceFill(false); // Not particularly silence.
              }
              
              jitterBuffer[inPos]->setIsLogicallyEmpty(true);
              jitterBuffer[inPos]->setIsInUse(true);
              
              incrementInPos();
              silencePatched++; //Accounting
              
              prevPacketRtpTime += sampleSize;
           }//while, do silence filling on missing slots.

           if ( prevPacketRtpTime != (pkt.getRtpTime() - sampleSize)) {
              cpLog( LOG_ERR, "Silent patching failed to correct rtptime"
                     "prevPacketRtpTime(%u), p->getRtpTime()(%u), networkPacketSize(%d)",
                     prevPacketRtpTime, pkt.getRtpTime(), sampleSize );

              prevPacketRtpTime = pkt.getRtpTime() - sampleSize;
              // On second thoughts, don't let a bogus packet kill us!
              // After a 1-2 day call, a Cisco phone dropped a few packets and
              // then started sampling at an 80-byte offset, from our
              // perspective.  The prevPacketRtpTime fixup above should get
              // us back in sync.
              //assert(0); // No excuse to be here. --Ben
           }

           // Add the real packet
           if (jitterBuffer[inPos] == NULL) {
              jitterBuffer[inPos] = new RtpData();
           }

           jitterBuffer[inPos]->setData(pkt.getPayloadLoc(), len);
           jitterBuffer[inPos]->setSilenceFill(false);
           jitterBuffer[inPos]->setIsLogicallyEmpty(false);
           jitterBuffer[inPos]->setIsInUse(true);

           incrementInPos();

           // update counters
           RtpSeqNumber tSeq = prevSeqRecv;
           prevSeqRecv++;
           if (prevSeqRecv > RTP_SEQ_MOD) {
              setPrevSeqRecv(0);
           }
           if (prevSeqRecv < tSeq) {
              cpLog(LOG_DEBUG_STACK, "Recv cycle");
              assert(prevSeqRecv == 0);
              recvCycles += RTP_SEQ_MOD;
           }

           prevPacketRtpTime = pkt.getRtpTime();

           if (faking) {
              // Try again next time ???
              silenceCodec = NULL;
           }
        }//if seq-number was in order
        else {
           // Find the index where we need to insert this.
           int idx = calculatePreviousInPos(RtpSeqDifference(prevSeqRecv, pkt.getSequence()));

           cpLog(LOG_ERR, "Inserting OOO packet at index: %d, prevSeqRecv: %d  pkt.seq: %d\n",
                 idx, prevSeqRecv, pkt.getSequence());

           if (jitterBuffer[idx] == NULL) {
              jitterBuffer[idx] = new RtpData();
           }

           jitterBuffer[idx]->setData(pkt.getPayloadLoc(), len);
           jitterBuffer[idx]->setSilenceFill(false);
           jitterBuffer[idx]->setIsLogicallyEmpty(false);
           jitterBuffer[idx]->setIsInUse(true);

           insertedOooPkt++;
        }//else

        packetReceived++;
        payloadReceived += len;

        // update jitter calculation
        packetTransit = arrival - rtp2ntp(pkt.getRtpTime());
        delay = packetTransit - lastTransit;
        lastTransit = packetTransit;

#ifdef USE_LANFORGE
        if (rtpStatsCallbacks) {
           uint64 now = arrival.getMs();
           rtpStatsCallbacks->avgNewJitterPB(now, delay, 1, len);
        }
#endif

        if (delay < 0) {
           delay = -delay;
        }

        // Found this on the web. --Ben
        //jitter- is calculated as for RCTP - RFC 1889
        //J = J + ( | D(i-1, i) | - J) / 16
        //where J is jitter and D is delay between two frames

        // The +8 helps mitigate rounding errors.
        jitter = jitter + (((delay - jitter) + 8) >> 4);

        pkt.clear();
    }//while

    // deque next packet
    if (inPos == playPos) {
       cpLog (LOG_ERR, "Recv buffer is empty");
       receiverError = recv_bufferEmpty;
       jitterBufferEmpty++; // Accounting
       return 0;
    }

    // create return value.
    pkt.clear();

    assert(jitterBuffer[playPos]);
    assert(jitterBuffer[playPos]->isInUse());

    int packetSize = jitterBuffer[playPos]->getCurLen();

    memcpy(pkt.getPayloadLoc(), jitterBuffer[playPos]->getData(), packetSize);
    pkt.setIsMissing(jitterBuffer[playPos]->isLogicallyEmpty());
    pkt.setIsSilenceFill(jitterBuffer[playPos]->isSilenceFill());

    // finish packet
    pkt.setSSRC (ssrc);
    pkt.setPayloadType (format);
    pkt.setPayloadUsage (packetSize);
    pkt.setRtpTime (prevRtpTime + sampleSize);
    pkt.setSequence (prevSeqPlay + 1);

/*
    cpLog(LOG_ERR, "New Packet, api: %d  size: %d seq: %d, playPos: %d, inPos: %d cur_max_jbs: %d",
          apiFormat, packetSize, prevSeqPlay + 1, playPos, inPos,
          cur_max_jbs);
*/

    if (probation > 0) {
       probation--;
    }
    receiverError = recv_success;

    prevRtpTime = pkt.getRtpTime();
    prevNtpTime = getNtpTime();

    incrementPlayPos();

    // update counters
    RtpSeqNumber sSeq = prevSeqPlay;
    seq = (int)(sSeq); /* Debugging, I see strange things here in gdb, maybe memory
                        * is being clobbered.
                        */
    prevSeqPlay = (prevSeqPlay + 1) % RTP_SEQ_MOD;
    if (prevSeqPlay < sSeq) {
       if (prevSeqPlay != 0) {
          cpLog(LOG_ERR, "Play cycle, prevSeqPlay: %d, sSeq: %d, seq: %d",
                prevSeqPlay, sSeq, seq);
          assert (prevSeqPlay == 0);
       }
       playCycles++;
    }

    return packetSize;
}


int RtpReceiver::getPacket(RtpPacket& pkt)
{
    // check for network activity

    // receive packet
    int len;
    len = myStack->receiveFrom ((char*)pkt.getHeader(), pkt.getPacketAlloc(), NULL,
                                MSG_DONTWAIT);
    if (len == 0) {
       cpLog(LOG_DEBUG_STACK, "NOTE:  Received -EAGAIN when trying to read rtp packet.\n");
       return 0;
    }

    pkt.setTotalUsage (len);
    cpLog(LOG_DEBUG_STACK, "RTP get packet len = %d, seq: %d rptTime: %d payloadUsage: %d",
          len, pkt.getSequence(), pkt.getRtpTime(), pkt.getPayloadUsage());

    // check packet
    if ( !pkt.isValid() ) {
        cpLog(LOG_ERR, "****Packet is not valid");
        return -EINVAL;
    }


    // check if rtp event
    if (pkt.getPayloadType() == rtpPayloadDTMF_RFC2833 ||
            pkt.getPayloadType() == rtpPayloadCiscoRtp)
    {
        if(getDTMFInterface() != 0)
        {
            //If a call-back is set, let the callback handle
            //the DTMF event
            recvEvent( pkt );
            return 0;
        }
        else
        {
            //Treat it as any other packet
            return len;
        }
    }

    // update receiver info
    //   function may return 1, meaning packet
    //   out of seq tolr or source not valid
    if (updateSource(pkt))
    {
        cpLog(LOG_ERR, "****Packet is discarded or source not valid");
        return -EINVAL;
    }

    return len;
}


int RtpReceiver::updateSource (RtpPacket& pkt)
{
    // check if ssrc in probation list
    if (sourceSet && (pkt.getSSRC() == srcProbation) && probationSet)
       // old probation packets still in que
       return 1;

    cpLog( LOG_DEBUG_STACK, "payload: %d, packet: %s",
           pkt.getPayloadUsage(), pkt.toString().c_str());
    //p->printPacket();
    
    // new source found or resync old source
    if ((!sourceSet) || (pkt.getSSRC() != ssrc))
    {
        if (addSource(pkt))
            return 1;
    }


#ifdef USE_LANFORGE
    if (rtpStatsCallbacks) {
       uint16 ts = pkt.getSequence();
       uint16 psr = realPrevSeqRecv;
       static uint16 one = 1;
       uint16 tm1 = (uint16)(ts - one);

       //cpLog(LOG_ERR, "RTP-seq-check, psr: %d  ts: %d, tm1: %d\n",
       //      psr, ts, tm1);

       if (packetReceived == 0) {
          goto lf_done;
       }
       
       if (psr == tm1) {
          // All is in order.
       }
       else {
          uint64 now = vgetCurMs();
          if (psr == ts) {
             rtpStatsCallbacks->notifyDuplicateRtp(now, 1);
          }
          else if (psr < tm1) {
             cpLog(LOG_ERR, "Dropped RTP (A), psr: %d  ts: %d, pktRcvd: %d\n",
                   psr, ts, packetReceived);
             rtpStatsCallbacks->notifyDroppedRtp(now, tm1 - psr);
          }
          else {
             // Maybe we wrapped and dropped at the same time?  Unlikely,
             // but check.
             if (psr > (ts + (short)(32000))) {
                // Was a wrap and drop, unlucky bastard!
                cpLog(LOG_ERR, "Dropped RTP (B), psr: %d  ts: %d, pktRcvd: %d\n",
                      psr, ts, packetReceived);
                rtpStatsCallbacks->notifyDroppedRtp(now, (uint16)((0xFFFF - psr)) + tm1);
             }
             else {
                // Must be out-of-order
                rtpStatsCallbacks->notifyOOORtp(now, 1);
             }
          }
       }
      lf_done:
       realPrevSeqRecv = ts;
    }
#endif

    // no vaild source yet
    assert (probation >= 0);

    // drop CN packets
    if ((pkt.getPayloadType() == rtpPayloadCN) ||
        (pkt.getPayloadType() == 13)) {
        cpLog(LOG_DEBUG_STACK, "drop 1 rtpPayloadCN packet");
        setPrevSeqRecv(pkt.getSequence());  // drop comfort noise packet
        comfortNoiseDropped++;
        return 1;
    }

    if (pkt.getPayloadType() != format ) {
        cpLog(LOG_DEBUG, "Transmitter changing payload parameters(%d/%d) (%d), packet: %s",
              pkt.getPayloadType(), format,
              pkt.getPayloadUsage(),
              pkt.toString().c_str());
        initSource(pkt);
    }

/*
    cpLog(LOG_DEBUG_STACK, "prevPacketRtpTime = %d", prevPacketRtpTime);
    cpLog(LOG_DEBUG_STACK, "p->getRtpTime() = %d", p->getRtpTime());
    cpLog(LOG_DEBUG_STACK, "it is %f", (( p->getRtpTime() - prevPacketRtpTime) * 1.0) / sampleSize);
*/

    // check if valid sequence window
    RtpSeqNumber seq = pkt.getSequence();
    //    cpLog(LOG_DEBUG, "seq = %d, prevSeqRecv = %d", seq, prevSeqRecv);
    if ( RtpSeqDifference(seq, prevSeqRecv) >= (int)(cur_max_jbs) ) { // way future packet
       // large sequence jump forward, skip over packets less then seq
       cpLog(LOG_DEBUG_STACK, "jump to %d (%d jumps)", seq, seq - prevSeqRecv);
       cpLog(LOG_DEBUG_STACK, "rtpTime %u, prevPacketRtpTime %u",
             pkt.getRtpTime(), prevPacketRtpTime);
       setPrevSeqRecv(seq - 1);
       prevPacketRtpTime = pkt.getRtpTime() - sampleSize;

       packetBadlyMisordered++; //Keep some stats.
    }
    else if ( RtpSeqGreater(prevSeqRecv, seq) ) { // past packet
       // check if the pkt is too late comparing to the pkt already
       // played. Since the prevSeqPlay is irrelavant due to diff
       // api_payload Size, use inPos and playPos to calculate.
       unsigned int backNoOfSeq = RtpSeqDifference(prevSeqRecv, seq);

       if (getJitterPktsInQueueCount() <= backNoOfSeq) {
          // Too late, discard
          cpLog(LOG_ERR, "DROPPING RTP packet, too late now: discard seq %d, prevSeq: %d, back: %d",
                seq, prevSeqRecv, backNoOfSeq);

          packetTooOld++; //Keep some stats.
          return 1;
       }
    }
    else { // (seq == prevSeqRecv)  // present packet
       // duplicate with previous packet
       //cpLog(LOG_DEBUG_STACK,"duplicate packet %d", seq);
       //return 1;
    }

    return 0;
}//updateSource



int RtpReceiver::addSource(RtpPacket& pkt)
{
    // don't allow ssrc changes without removing first
    if (sourceSet)
    {
        if (probation < 4)
        {
            probation ++;
            cpLog(LOG_ERR, "Rejecting new transmitter %u, keeping %u",
                  pkt.getSSRC(), ssrc);
            return 1;
        }
        else {
           removeSource(ssrc);
        }
    }

    // check if ssrc in probation list
    if (sourceSet && (pkt.getSSRC() == srcProbation) && probationSet)
        return 1;

    sourceSet = true;
    ssrc = pkt.getSSRC();
    cpLog(LOG_DEBUG_STACK, "Received ssrc = %u", ssrc);
    probation = 0;
    packetReceived = 0;
    payloadReceived = 0;


    // init SDES and RTCP fields
    if (rtcpRecv)
       rtcpRecv->addTranInfo(ssrc, this);

    initSource (pkt);

    return 0;
}



void RtpReceiver::initSource (RtpPacket& pkt)
{
    assert (ssrc == pkt.getSSRC());

    cpLog(LOG_DEBUG_STACK, "InitSource %u with sequence %d and rtp time %u",
          ssrc, pkt.getSequence(), pkt.getRtpTime());
    seedSeq = pkt.getSequence();
    seedNtpTime = getNtpTime();
    seedRtpTime = pkt.getRtpTime();

    // set receiving codec

    inPos = 0;
    playPos = 0;

    // set timing information
    prevRtpTime = pkt.getRtpTime() - sampleSize;
    prevPacketRtpTime = pkt.getRtpTime() - sampleSize;
    prevNtpTime = rtp2ntp(pkt.getRtpTime()) - sampleSize * 1000 / clockRate;
    setPrevSeqRecv(pkt.getSequence() - 1);
    recvCycles = 0;
    playCycles = 0;

    lastTransit = 0;
    jitter = 0;
}


void RtpReceiver::removeSource (RtpSrc s, int flag)
{
    if (s != ssrc)
        cpLog(LOG_DEBUG_STACK, "Removing non active source: %u", s);

    // no longer listen to this source
    probationSet = true;
    srcProbation = s;

    // no transmitter
    sourceSet = false;
    ssrc = 0;
    probation = -2;

    // remove from RTCP receiver
    if (rtcpRecv && !flag) rtcpRecv->removeTranInfo (s, 1);

    cpLog (LOG_DEBUG_STACK, "Removing source: %u", s);
}



NtpTime RtpReceiver::rtp2ntp (const RtpTime& rtpTime)
{
    NtpTime ntptime = seedNtpTime +
                      ((rtpTime - seedRtpTime) * 1000 / clockRate);
    return ntptime;
}


void RtpReceiver::clearBuffer() {
   for (int i = 0; i<JITTER_BUFFER_MAX; i++) {
      if (jitterBuffer[i]) {
         delete jitterBuffer[i];
         jitterBuffer[i] = NULL;
      }
   }
}
 
 
void RtpReceiver::printBuffer(int err_level) {
   for (int i = 0; i<JITTER_BUFFER_MAX; i++) {
      if (jitterBuffer[i]) {
         cpLog(err_level, "[%d] %s\n", jitterBuffer[i]->toString().c_str());
      }
   }
}

/* --- Session state functions ------------------------------------- */
void RtpReceiver::emptyNetwork ()
{

    // create empty holder packet
   char buf[1500];

   while (myStack->receive (buf, 1500, MSG_DONTWAIT) > 0) {
      // Grab another
   }

    cpLog (LOG_DEBUG_STACK, "RtpReceiver: Done empty network queue");
}


/* --- Private Information for RTCP -------------------------------- */

void RtpReceiver::setRTCPrecv (RtcpReceiver* s)
{
    rtcpRecv = s;
}


void RtpReceiver::setFormat (RtpPayloadType newtype) {
    format = newtype;
}


void RtpReceiver::setCodecString(const char* codecStringInput)
{
    strncpy(codecString, codecStringInput, strlen(codecStringInput) + 1);
    cpLog(LOG_DEBUG, "set CodecString %s", codecString);
}


void RtpReceiver::incrementInPos() {
   inPos++;
   if (inPos >= cur_max_jbs) {
      inPos = 0;
   }
}

void RtpReceiver::incrementPlayPos() {
   playPos++;
   if (playPos >= cur_max_jbs) {
      playPos = 0;
   }
}

unsigned int RtpReceiver::calculatePreviousInPos(int packets_ago) const {
   unsigned int tmp = packets_ago % cur_max_jbs;
   
   if (tmp > inPos) {
      // Wrap
      tmp -= inPos;
      return cur_max_jbs - tmp;
   }
   else {
      return inPos - tmp;
   }
}

unsigned int RtpReceiver::getJitterPktsInQueueCount() const {
   if (inPos == playPos) {
      return 0;
   }

   if (inPos < playPos) {
      // Must be a wrap
      return (cur_max_jbs - playPos) + inPos;
   }
   else {
      return inPos - playPos;
   }
}
