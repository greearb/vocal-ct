#ifndef RTPTRANSMITTER_HXX
#define RTPTRANSMITTER_HXX

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000-2003 Vovida Networks, Inc.  All rights reserved.
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

#include <sys/types.h>
#include <map>

#include "rtpTypes.h"
#include "UdpStack.hxx"
#include "NtpTime.hxx"

#include "NetworkAddress.h"

class RtpPacket;
class RtcpReceiver;


/**
   The sending side of an RTP session.

   @see RtpSession
*/

class RtpTransmitter
{
public:

   /** constructor using port range
    * local_ip can be "" if you wish to use the kernel default
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    * receiver can be NULL.
    */
   RtpTransmitter (const char* debug_msg, uint16 tos, uint32 priority,
                   const string& local_ip,
                   const string& local_dev_to_bind_to,
                   const char* remoteHost, int remoteMinPort,
                   int remoteMaxPort,
                   RtpPayloadType format, int clockrate,
                   int per_sample_size, int samplesize,
                   RtpReceiver* receiver);

   /** construtor using specified port
    * local_ip can be "" if you wish to use the kernel default
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    * remote port used to default to 5000
    * receiver can be NULL.
    */
   RtpTransmitter (const char* debug_msg, uint16 tos, uint32 priority,
                   const string& local_ip,
                   const string& local_dev_to_bind_to,
                   const char* remoteHost, int remotePort,
                   RtpPayloadType format, int clockrate,
                   int per_sample_size, int samplesize,
                   RtpReceiver* receiver);

   ///
   virtual ~RtpTransmitter ();

   /// Creates a packet with transmitter's payload type and SRC number
   RtpPacket* createPacket (int npadSize = 0, int csrc_count = 0);

   /// clear packet before re-using.
   int transmit (RtpPacket& packet, bool eventFlag = false);
   ///

   // takes rawdata, pokes it in an RTP packet, and sends it.
   // It does no additional buffering, the data should be exactly
   // enough for one RTP payload.  Cannot otherwise work with variable-sized
   // codecs. --Ben
   int transmitRaw (const char* buffer, int data_len);

   /** Let the stack know we are suppressing an RTP packet send
    * due to VAD...for accounting purposes mainly.
    */
   int notifyVADSuppression(int len);
   

   ///
   RtpSrc getSSRC () { return ssrc; }
   ///
   void setSSRC (RtpSrc src) { ssrc = src; }

   ///
   int getPacketSent () const { return packetSent; }
   ///
   int getPayloadSent () const { return payloadSent; }

   ///
   RtpSeqNumber getPrevSequence () const { return prevSequence; }

   ///
   RtpTime getPrevRtpTime () const { return prevRtpTime; }

   ///
   void setMarkerOnce() { markerOnce = true; }

   ///
   void setCodecString (const char* codecStringInput);

   /// Port this stack is sending its singal
   int getPort () const { return myStack->getTxPort(); }

   /// Socket File Descriptor used for select()
   int getSocketFD () { return myStack->getSocketFD(); }

   /// transmitter error code;
   RtpTransmitterError transmitterError;

   /// get ptr of my UdpStack
   Sptr<UdpStack> getUdpStack() { return myStack; }

   ///
   NetworkAddress* getRemoteAddr () { return &remoteAddr; }

   int getClockRate() const { return clockRate; }
   int getPerSampleSize() const { return perSampleSize; }
   int getSampleSize () { return sampleSize; }
   RtpPayloadType getFormat() const { return format; }

   void setSampleSize (int size) { sampleSize = size; }
   void setFormat (RtpPayloadType type);
   void setClockRate(int c) { clockRate = c; }
   void setPerSampleSize(int s) { perSampleSize = s; }

   void setRemoteAddr (const NetworkAddress& theAddr) ;

   void initRtpTransmitter (RtpPayloadType format, int clockrate,
                            int per_sample_size, int samplesize);


private:

   /// local SRC number
   RtpSrc ssrc;
   /// inital NTP time
   NtpTime seedNtpTime;
   /// inital RTP time
   RtpTime seedRtpTime;
   /// RTP time of previous packet
   RtpTime prevRtpTime;
   /// NTP time of previous packet
   NtpTime prevNtpTime;
   /// previous packet's sequence
   RtpSeqNumber prevSequence;

   /// numbers of packet sent
   int packetSent;
   /// total bytes of payload sent
   int payloadSent;

   /// allow RTCP stack to probe for information
   friend class RtcpTransmitter;
   /// allow RTCP stack to probe for information
   friend class RtcpReceiver;

   /// format of payload for stack
   RtpPayloadType format;

   /// codecString in case of dynamic payload type
   char codecString[32];

   /// number of samples per RTP packet on api (typical 160 or 240)
   int sampleSize;

   /// number of bytes per sample  (typical 1 or 2, used for endian conversion)
   int perSampleSize;

   /// payload specific sample rate
   int clockRate;

   /// marker once
   bool markerOnce;

   /// my UDP stack
   Sptr<UdpStack> myStack;

   NetworkAddress remoteAddr;

   // This guy saves us having to allocate and delete packets when sending raw.
   RtpPacket rtp_raw_tx_pkt;
};


#endif // RTPTRANSMITTER_HXX
