/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2002-2003 Vovida Networks, Inc.  All rights reserved.
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
#include <cassert>
#include "RtpPacket.hxx"

#include "MediaDevice.hxx"
#include "MediaSession.hxx"
#include "MRtpSession.hxx"
#include "MediaController.hxx"
#include "misc.hxx"

using namespace Vocal::UA;
using namespace Vocal::SDP;


MRtpSession::MRtpSession(int sessionId, NetworkRes& local,
                         uint16 tos, uint32 priority,
                         const string& local_dev_to_bind_to,
                         NetworkRes& remote ,
                         Sptr<CodecAdaptor> cAdp, int rtpPayloadType,
                         u_int32_t ssrc,
                         VADOptions *_vadOptions, int jitter_buffer_sz)
    : Adaptor("MRtpSession", "MRtpSession", RTP, NONE), mySessionId(sessionId),
      rtp_rx_packet(1012),
      consecutiveSilentSamples(0),
      consecutiveSilentSentSamples(0),
      vadOptions(_vadOptions)
{

    lastRtpRetrieve = 0;
    myCodec = cAdp;
    myLocalAddress = new NetworkRes(local);
    myRemoteAddress = new NetworkRes(remote);
    localDevToBindTo = local_dev_to_bind_to;
    _tos = tos;
    _skb_priority = priority;

    int remotePort = myRemoteAddress->getPort();
    int localPort  = myLocalAddress->getPort();
    int remoteRtcpPort = ( remotePort > 0) ? remotePort + 1 : 0;
    int localRtcpPort = (localPort > 0) ? localPort + 1 : 0;

    int pType = convertAdapterTypeToRtpType(cAdp->getType(), rtpPayloadType);

    rtpStack =  new RtpSession(tos, priority, myLocalAddress->getIpName().c_str(),
                               localDevToBindTo,
                               myRemoteAddress->getIpName().c_str(), 
                               remotePort,
                               localPort,
                               remoteRtcpPort,
                               localRtcpPort, (RtpPayloadType)(pType),
                               cAdp->getClockRate(), cAdp->getPerSampleSize(),
                               cAdp->getSampleSize(), jitter_buffer_sz);

    myDTMFInterface = new MDTMFInterface(this);
    rtpStack->setDTMFInterface(myDTMFInterface);
    rtpStack->setMarkerOnce();
}


int MRtpSession::convertAdapterTypeToRtpType(VCodecType ct, int negotiatedRtpPayloadType) {
   int pType;  // 96 <-> 127 are dynamic, doesn't map well to an enum.
   switch(ct) {
   case G711U:
      pType = rtpPayloadPCMU;
      break;
   case G711A:
      pType = rtpPayloadPCMA;
      break;
   case G729:
      pType = rtpPayloadG729;
      break;
   case G726_16:
      pType = rtpPayloadG726_16;
      break;
   case G726_24:
      pType = rtpPayloadG726_24;
      break;
   case G726_32:
      pType = rtpPayloadG726_32;
      break;
   case G726_40:
      pType = rtpPayloadG726_40;
      break;
   case SPEEX:
      pType = negotiatedRtpPayloadType; // Negotiated
      assert((pType >= rtpPayloadDynMin) && (pType <= rtpPayloadDynMax));
      break;
   default:
      pType = rtpPayloadPCMU;
      break;
   }//switch
   return pType;
}


void MRtpSession::readNetwork(fd_set* fds) {
    RtpSessionState sessionState = rtpStack->getSessionState();
    
    if ( sessionState == rtp_session_undefined ) {
        cpLog(LOG_ERR, "Session state undefined, returning");
        return;
    }


    // Generally, we receive 1 pkt every 20ms, so only going to try to read one
    // pkt at a time here.  If there is more to be received, the select will cause
    // us to return here quickly... --Ben

    rtpStack->readNetwork(fds);
}//readNetwork

MRtpSession::~MRtpSession() {
    delete myRemoteAddress;
    delete rtpStack;
    delete myLocalAddress;
    delete myDTMFInterface;
}

void MRtpSession::retrieveRtpSample() {
   rtp_rx_packet.clear();
   
   int rv = rtpStack->retrieve(rtp_rx_packet, "retrieveRtpSample");
   
   if (rv > 0) {
      /* Removing check for payload-usage.  It doesn't work for variable
       * encoding schemes. --Ben
       */
      if (rtp_rx_packet.isSilenceFill() || (rtp_rx_packet.getPayloadType() == (RtpPayloadType)myCodec->getRtpType())) {
         processRecv(rtp_rx_packet, *myRemoteAddress);
      }
      else {
         cpLog(LOG_ERR, "Received from RTP stack incorrect payload type, rtp_type: %d  codec_type: %d  codec_rtp_type: %d",
               rtp_rx_packet.getPayloadType(), myCodec->getType(), myCodec->getRtpType());
      }
   }//if
   else {
      cpLog(LOG_DEBUG_STACK, "failed to rtpStack->retrieve, rv: %d\n", rv);
   }
}//retrieveRtpSample


void MRtpSession::flushJitterBuffer() {
   while (rtpStack->getRtpRecv()->getJitterPktsInQueueCount()) {
      retrieveRtpSample();
   }
}


void MRtpSession::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                       uint64 now) {
   readNetwork(input_fds); /** Handles both RTCP and RTP receive logic,
                            * will not block. */

   if ((mySession == 0) || (mySessionId != mySession->getSessionId())) {
      mySession = MediaController::instance().getSession(mySessionId);
   }
   
   //cpLog(LOG_DEBUG_STACK, "MrtpSession::tick, mySession: %p", mySession.getPtr());
   // Only drain jitter buffer every XXX miliseconds.
   if (mySession.getPtr()) {
      uint64 pref = mySession->getPerPacketTimeMs();
      if (lastRtpRetrieve == 0) {
         lastRtpRetrieve = now;
      }
      if ((lastRtpRetrieve + pref) <= now) {
         retrieveRtpSample();
         lastRtpRetrieve += pref;
      }
      else {
         //cpLog(LOG_ERR, "Too soon to retrieve sample, lastRtpRetrieve: %llu  pref: %llu  now: %llu",
         //      lastRtpRetrieve, pref, now);
      }
   }
}//tick

int MRtpSession::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                        int& maxdesc, uint64& timeout, uint64 now) {

   rtpStack->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);

   if ((mySession == 0) || (mySessionId != mySession->getSessionId())) {
      mySession = MediaController::instance().getSession(mySessionId);
   }
   
   if (mySession.getPtr()) {
      uint64 pref = mySession->getPerPacketTimeMs();
      if (lastRtpRetrieve == 0) {
         lastRtpRetrieve = now;
      }
      if (pref + lastRtpRetrieve > now) {
         timeout = min(timeout, (pref + lastRtpRetrieve) - now);
      }
      else {
         timeout = 0;
      }
   }
   return 0;
}//setFds


void MRtpSession::processRecv(RtpPacket& packet, const NetworkRes& sentBy) {
   if ((mySession == 0) || (mySessionId != mySession->getSessionId())) {
      mySession = MediaController::instance().getSession(mySessionId);
   }

   if (mySession == 0) {
      cpLog(LOG_ERR, "ERROR:  Could not get session: %d\n", mySessionId);
   }
   else {
      //cpLog(LOG_ERR, "MRtpSession::procesRecv\n");
      mySession->processRaw((char*) packet.getPayloadLoc(), packet.getPayloadUsage(),
                            myCodec->getType(), myCodec, this,
                            packet.isSilenceFill(), NULL);
   }
}

// Just a little routine to check whether the buffer contains silence.
//
// This method assumes that the data being processed is the optimum encoding
// for voice quality. 
// That is: 16-bit signed, mono, 8000Hz (8Khz), PCMU (or in LF terms G711U)
//
// Q: What is the most effective way of detecting silence?
// A: Spoke to my mate Stephen (stephen@blacksapphire.com)
//    and he told me that this was the best method
//    "Square your threshold, and compare the square sum against the 
//     squared threshold"
//
//  As I trust him - I guess that is exactly what I'll do
//     -- Ben Martel (benm@symmetric.co.nz)
bool MRtpSession::isSilence(char *buffer, int noOfSamples, int perSampleSize)
{
   int32_t i = 0;
   int32_t total_of_values = 0;
   int32_t average = 0;
   uint64 sumOfSquares = 0;
   uint64 thresholdComparisonValue;
   int16_t* samples;

   if (perSampleSize == 2) {
      samples = reinterpret_cast<int16_t*>(buffer);
   }
   else {
      // If we have been handed ulaw data, then we don't handle this yet.
      // Consider converting it.
      cpLog(LOG_DEBUG_STACK, "mu-law data - ahhhhhhhh! ");
      return false;
   }

   for (i=0; i<noOfSamples; i++) {
      total_of_values += (int32_t)samples[i];
   }

   average = total_of_values / noOfSamples;

   for (i=0; i<noOfSamples; i++) {
      int32_t deviation = (int32_t)samples[i] - average;
      sumOfSquares += (uint64) ((uint64)deviation*(uint64)deviation);
   }

   // The value here of 0x7e equates to -37dB which is the value
   // chosen by the ITU
   thresholdComparisonValue = (uint64)0x7e * (uint64)0x7e * (uint64)noOfSamples;
   if (sumOfSquares < thresholdComparisonValue)
      return true;  // It is silence....sshhhhh!
   else
      return false; // Nope - something was making a noise.
}

// This assumes that the data is already right for myCodec.
// Use sinkData if that is not the case.
int MRtpSession::sinkCooked(const char* data, int length, int samples,
                            VCodecType type) {
   // Check for silence pkt, ie VAD
   if (length == 0) {
      consecutiveSilentSentSamples += samples;
      rtpStack->notifyVADSuppression(length);
      return 0;
   }
   else {
      if (type != myCodec->getType()) {
         cpLog(LOG_ERR, "ERROR:  Invalid type: %d  codec->type: %d\n",
               type, myCodec->getType());
         return -1;
      }
      else {
         consecutiveSilentSamples = 0;
         consecutiveSilentSentSamples = 0;
         return rtpStack->transmitRaw(data, length);
      }
   }
}//sinkCooked


///Consume the raw data
void 
MRtpSession::sinkData(char* data, int length, VCodecType type,
                      Sptr<CodecAdaptor> codec, bool silence_pkt,
                      RtpPayloadCache* payload_cache) {

   //if the codec type is not same as the network type of the session
   //convert back, say for example if data is PCMU and sent over session codec
   //is G729, convert data first to G729. If G729 data is coming as input, no converssion
   //is needed and send it as is
   //Send RTP packet to the destination
   cpLog(LOG_DEBUG_STACK, "%s sinking data", description().c_str());
   cpLog(LOG_DEBUG_STACK, "type = ");
   int retVal = 0;
   if (type == DTMF_TONE) {
      //First 8 bits will represent the event
      char event;
      memcpy(&event, data, 1);
      cerr << "Sending digit:" << (int)event << endl;
      retVal = rtpStack->transmitEvent(event);
   }
   else {
      if (type != myCodec->getType() || vadOptions->getVADOn()) {
         //First convert the data from type to myType 
         cpLog(LOG_DEBUG_STACK, "Converting outgoing data, type: %d  myCodec.type: %d, codec: %p",
               type, myCodec->getType(), codec.getPtr());
         
         if (codec.getPtr() == 0) {
            codec = MediaController::instance().getMediaCapability().getCodec(type);
         }
         
         //Convert from codec type to PCM
         int decLen = 1024;
         char decBuf[1024];
         int decodedSamples = 0;
         int decodedPerSampleSize = 0;
            
         //string dbg;
         //vhexDump(data, length, dbg);
         //cpLog(LOG_ERR, "Before decode: %s", dbg.c_str());
         
         codec->decode(data, length, decBuf, decLen, decodedSamples,
                       decodedPerSampleSize, false);
         //vhexDump(data, decLen, dbg);
         //cpLog(LOG_ERR, "After decode: %s", dbg.c_str());

         bool silentPacket = false;
         bool suppress = false;
         if (vadOptions->getVADOn()) {
            silentPacket = isSilence(decBuf, decodedSamples, decodedPerSampleSize);

            if (silentPacket) {
               consecutiveSilentSamples += decodedSamples;
            }
            else {
               consecutiveSilentSamples = 0;
            }

            uint64 consecSilenceMs = (consecutiveSilentSamples * 1000) / codec->getClockRate();
            suppress = consecSilenceMs >= vadOptions->getVADMsBeforeSuppression();
         }
         
         // However, if we have not been tramsitting RTP for a while (suppressing)
         // because there has been a *long* period of silence (this can 
         // happen when someone mutes their handset for example)
         // we need to actually send something as other code
         // might think that the RTP session has died. 
         // To do this, we check to see if the suppression has been in force
         // for 4 times longer than the time we waited before applying it.
         if (suppress) {
            uint64 consecSilenceSentMs = (consecutiveSilentSentSamples * 1000) / codec->getClockRate();
            if (consecSilenceSentMs >= vadOptions->getForceSendMs()) {
               cpLog(LOG_ERR, "Forcing send of pkt even though we are in VAD..\n");
               suppress = false;
            }
         }
         
         if (suppress) {
            // Let's tell lower layer for accounting purposes, etc.
            consecutiveSilentSentSamples += decodedSamples;
            rtpStack->notifyVADSuppression(length);
            if (payload_cache) {
               // Add a 'silent' pkt
               payload_cache->addRtpPldBuffer(NULL, 0, 0, myCodec->getType());
            }
         }
         else {
            consecutiveSilentSentSamples = 0;
            // If the input and output codecs are the same, then we transmit the
            // input data instead of re-encoding.
            if (type == myCodec->getType()) {
               if (payload_cache) {
                  payload_cache->addRtpPldBuffer(data, length, 160,
                                                 myCodec->getType());
               }
               retVal = rtpStack->transmitRaw(data, length);
            }
            else {
               int encLen = 1024;
               char encBuf[1024];
               myCodec->encode(decBuf, decodedSamples, decodedPerSampleSize,
                               encBuf, encLen);
               
               //vhexDump(data, encLen, dbg);
               //cpLog(LOG_ERR, "After encode: %s", dbg.c_str());
               
               if (encLen > 0) { 
                  retVal = rtpStack->transmitRaw(encBuf, encLen);

                  if (payload_cache) {
                     payload_cache->addRtpPldBuffer(encBuf, encLen, decodedSamples,
                                                    myCodec->getType());
                  }
               }
               else { 
                  // Don't trasmit if there has been an error
                  cpLog(LOG_ERR, "Coding error, no data returned from codec");
               }
            }
         }
      }
      else {
         consecutiveSilentSamples = 0;
         consecutiveSilentSentSamples = 0;
         //string dmp;
         //hexDump(data, length, dmp);
         //cpLog(LOG_DEBUG_STACK, "Transmit raw:%d\n%s", length, dmp.c_str());
         retVal = rtpStack->transmitRaw(data, length);

         if (payload_cache) {
            payload_cache->addRtpPldBuffer(data, length, 160,
                                           myCodec->getType());
         }
      }
   }

   if (retVal < 0) {
      cpLog(LOG_ERR, "Failed to transmit RTP packets to %s:%d", 
            myRemoteAddress->getIpName().c_str(),
            myRemoteAddress->getPort());
   }
}//sinkData


string MRtpSession::description() {
    char buf[256];
    snprintf(buf, 255, "%s:%d->%s:%d", myLocalAddress->getIpName().c_str(),
             myLocalAddress->getPort(),
             myRemoteAddress->getIpName().c_str(),
             myRemoteAddress->getPort());
    return buf;
}

void MRtpSession::recvDTMF(int event) {
   cpLog(LOG_DEBUG_STACK, "Received DTMF: %d\n", event);
   char eventData = event;
   Sptr<CodecAdaptor> nll;
   
   if ((mySession == 0) || (mySessionId != mySession->getSessionId())) {
      mySession = MediaController::instance().getSession(mySessionId);
   }
   if (mySession.getPtr()) {
      mySession->processRaw(&eventData, 1, DTMF_TONE, nll, this, false, NULL);
   }
}

int MRtpSession::adopt(SdpSession& localSdp, SdpSession& remoteSdp) {

   // Set remote host and port
   //   For now only support unicast
   string remAddr;
   string localAddr;
   LocalScopeAllocator lo;
   if (localSdp.getConnection())
      localAddr = localSdp.getConnection()->getUnicast().getData(lo);
   if (remoteSdp.getConnection())
      remAddr = remoteSdp.getConnection()->getUnicast().getData(lo);

   cpLog(LOG_ERR, "adopt:  localAddr: %s  remAddr: %s", localAddr.c_str(), remAddr.c_str());

   //Get the Negotiated Codec for each media type
   //Get each media description from remoteSdp and map it to the
   //localSdp SDP and create an MRtpSession
   list < SdpMedia* > mList = remoteSdp.getMediaList();
   int fmt = 0;
   for (list < SdpMedia* >::iterator itr = mList.begin();
        itr != mList.end(); itr++) {
      SdpMedia* rMedia = (*itr);
      string rAddr;
      string lAddr;
      int lPort = -1;
      int rPort;
      int sample_rate = 8000;

      Sptr<CodecAdaptor> cAdp;
      rPort = rMedia->getPort();
      list < SdpMedia* > lmList = localSdp.getMediaList(); 
      for (list < SdpMedia* >::iterator itr2 = lmList.begin();
           itr2 != lmList.end(); itr2++) {
         SdpMedia* lMedia = (*itr2);
         if (lMedia->getMediaType() != rMedia->getMediaType()) {
            continue;
         }
         //Get the very first codec of the media description
         vector < int > * lfmList = lMedia->getFormatList();
         if (!lfmList || (lfmList->size() == 0)) {
            continue;
         }

         vector < int > * rfmList = rMedia->getFormatList();
         if (!rfmList || (rfmList->size() == 0)) {
            continue;
         }
         
         // So, we put our favorites first.  Assume the remote does
         // the same.  So, find the codec that matches the remote's preference.
         for (unsigned int q = 0; q<rfmList->size(); q++) {
            for (unsigned int r = 0; r<lfmList->size(); r++) {
               if ((*rfmList)[q] == (*lfmList)[r]) {
                  fmt = (*rfmList)[q];
                  goto found_one;
               }
            }
         }

         // The goto will jump over this if we actually find a match.
         continue;
               
        found_one:
         
         cAdp = MediaSession::getCodecAdaptor(fmt, lMedia, sample_rate);
         
         if (lMedia->getConnection()) {
            LocalScopeAllocator lo;
            lAddr = lMedia->getConnection()->getUnicast().getData(lo);
         }
         else {
            lAddr = localAddr;
         }
         lPort = lMedia->getPort();
         break;
      }//for
      if (rMedia->getConnection()) {
         LocalScopeAllocator lo;
         rAddr = rMedia->getConnection()->getUnicast().getData(lo);
         cpLog(LOG_ERR, "rMedia has connection, rAddr: %s", rAddr.c_str());
      }
      else {
         rAddr = remAddr;
      }
      
      //Now if we have lAddr, rAddr, lport, rPort and codec adaptor
      if ((lAddr.size() ==0) || (rAddr.size() == 0) || (cAdp == 0)) {
         cpLog(LOG_ERR, "Media is not setup correctly, lAddr: %s  rAddr: %s  cAdp: %p, fmt: %d\n",
               lAddr.c_str(), rAddr.c_str(), cAdp.getPtr(), fmt);
         continue;
      }

      // Good to go.
      cpLog(LOG_ERR, "rAddr: %s  rPort: %d", rAddr.c_str(), rPort);

      myRemoteAddress->setHostName(rAddr);
      myRemoteAddress->setPort(rPort);

      int remotePort  = myRemoteAddress->getPort();
      int remoteRtcpPort = ( remotePort > 0) ? remotePort + 1 : 0;
      rtpStack->setSessionState(rtp_session_sendrecv);

      rtpStack->setTransmiter(_tos, _skb_priority,
                              myLocalAddress->getIpName().c_str(), localDevToBindTo,
                              myRemoteAddress->getIpName().c_str(), remotePort,
                              remoteRtcpPort,
                              (RtpPayloadType)(convertAdapterTypeToRtpType(cAdp->getType(), fmt)),
                              cAdp->getClockRate(), cAdp->getPerSampleSize(),
                              cAdp->getSampleSize());
      return 0;
   }//for

   cpLog(LOG_ERR, "ERROR:  in MRtpSession::adopt, couldn't find supproted media.");
   return -1;
}//adopt

void MRtpSession::setMode(VSdpMode mode) {
   switch (mode) {
   case VSDP_SND:
      rtpStack->setSessionState(rtp_session_sendonly);
      break;
   case VSDP_RECV:
      rtpStack->setSessionState(rtp_session_recvonly);
      break;
   case VSDP_SND_RECV:
      rtpStack->setSessionState(rtp_session_sendrecv);
      break;
   }
}
