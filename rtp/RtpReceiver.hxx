#ifndef RTPRECEIVER_HXX
#define RTPRECEIVER_HXX

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

static const char* const RtpReceiver_hxx_Version =
    "$Id: RtpReceiver.hxx,v 1.3 2004/06/16 06:51:25 greear Exp $";


#include <sys/types.h>
#include <map>

#include "Sptr.hxx"

#include "rtpTypes.h"
#include "UdpStack.hxx"
#include "RtpEvent.hxx"
#include "NtpTime.hxx"
#include "RtpEventReceiver.hxx"

#include "NetworkAddress.h"

class RtpPacket;
class RtcpReceiver;


/** Used for jitter-buffer */
#define MAX_RTP_DATA 2048      /* Max size of the data in one packet */
#define JITTER_BUFFER_MAX 1024 /* Max number of packets in our jitter buffer */

class RtpData {
protected:
   int len;
   char data[MAX_RTP_DATA];

   bool is_silence_fill;
   bool is_logically_empty; // Some codecs want to generate fill themselves. 
   bool is_in_use; //Ie, does this sample logically exist?

public:
   RtpData() : len(0), is_silence_fill(false),
               is_logically_empty(false), is_in_use(false) { }

   void setData(const char* incomming_data, int data_len) {
      assert(data_len <= MAX_RTP_DATA);
      memcpy(data, incomming_data, data_len);
      len = data_len;
   }
   void setSilenceFill(bool b) { is_silence_fill = b; }
   void setIsInUse(bool b) { is_in_use = b; }
   void setIsLogicallyEmpty(bool b) { is_logically_empty = b; }

   bool isInUse() const { return is_in_use; }
   bool isLogicallyEmpty() const { return is_logically_empty; }
   bool isSilenceFill() const { return is_silence_fill; }

   int getCurLen() const { return len; }
   const char* getData() const { return data; }

   string toString() const;
};

/** 
    The receiving side of the RTP session.

    @see RtpSession
*/
class RtpReceiver : public RtpEventReceiver {
public:

   /** using specified port
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    */
   RtpReceiver (const string& local_ip,
                const string& local_dev_to_bind_to,
                int localPort,
                RtpPayloadType _format, int _clockrate, int per_sample_size,
                int samplesize);

   /** using port range
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    */
   RtpReceiver (const string& local_ip,
                const string& local_dev_to_bind_to,
                int localMinPort, int localMaxPort,
                RtpPayloadType _format, int _clockrate, int per_sample_size,
                int samplesize);

   /** Giving UdpStack ptr */
   RtpReceiver (Sptr<UdpStack> udp, RtpPayloadType _format,
                int _clockrate, int per_sample_size,
                int samplesize);

   /// consturctor init (don't call this function)
   void constructRtpReceiver (RtpPayloadType _format,
                              int clockrate, int per_sample_size,
                              int samplesize);
   ///
   virtual ~RtpReceiver ();


   /** Receive an RTP packet from buffer.
       pkt is passed in, and is expected to be ready to receive a pkt
       @return <= 0 implies one should not inspect pkt, nothing worth receiving
       was received.
   **/
   int receive (RtpPacket& pkt, fd_set* fds);


   int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
              int& maxdesc, uint64& timeout, uint64 now);


   /** Receive an RTP packet from buffer.
       pkt is passed in, and is expected to be ready to receive a pkt
       @return <= 0 implies one should not inspect pkt, nothing worth receiving
       was received.
   **/
   int getPacket (RtpPacket& pkt);

   /** Update souce information
    *  @return 0 not valid, 1 valid
    **/
   int updateSource (RtpPacket& packet);

   /// Initalize source counters
   void initSource (RtpPacket& packet);

   /** Add source to receiver
    *  @return 1 if transmitter on probation
    **/
   int addSource (RtpPacket& packet);

   /// Remove source from receiver
   void removeSource (RtpSrc src, int flag = 0);


   /** Convert from rtp time to ntp time
    *  using inital ntp time and sampleRate
    **/
   NtpTime rtp2ntp (const RtpTime& time);

   /// Clears out jitter buffer
   void clearBuffer();

   /// Prints out the jitter buffer.
   void printBuffer(int log_level);

   /// Empties pending network packets
   void emptyNetwork ();

   ///
   void setRTCPrecv (RtcpReceiver* rtcpRecv);

   ///
   void setCodecString (const char* codecStringInput);


   int getClockRate() const { return clockRate; }
   int getPerSampleSize() const { return perSampleSize; }
   int getSampleSize () { return sampleSize; }
   RtpPayloadType getFormat() const { return format; }

   void setSampleSize (int size) { sampleSize = size; }
   void setFormat (RtpPayloadType type);
   void setClockRate(int c) { clockRate = c; }
   void setPerSampleSize(int s) { perSampleSize = s; }

   int getPacketReceived () const { return packetReceived; }
   int getPayloadReceived () const { return payloadReceived; }
   int getJitter () const { return jitter; }

   unsigned long invalidPldSize;
   unsigned long sidPktsRcvd;

   /// Port this stack is sending its singal
   int getPort () const { return myStack->getRxPort(); }

   /// receiver error code;
   RtpReceiverError receiverError;

   /// get ptr of my UdpStack
   Sptr<UdpStack> getUdpStack() { return myStack; }

   bool isSpeexFormat() const { return is_speex; }
   void setIsSpeexFormat(bool b) { is_speex = b; }

   unsigned int getJitterPktsInQueueCount() const;
   unsigned int getCurMaxPktsInQueue() const { return cur_max_jbs; }

protected:

   unsigned int cur_max_jbs; // How big (logically) is our jitter buffer
   RtpData* jitterBuffer[JITTER_BUFFER_MAX];

   /// max number of sequential lost packets allowed
   int seq_loss_max;

   // Is this guy receiving for the Speex protocol?
   bool is_speex;

   /// source found flag
   bool sourceSet;
   /// SRC number for this source
   RtpSrc ssrc;
   /// probation set flag
   bool probationSet;
   /// wouldn't listen to this source
   RtpSrc srcProbation;
   /// probation, 0 source valid
   int probation;

   /// inital seqence number
   RtpSeqNumber seedSeq;
   /// inital NTP timestamp
   NtpTime seedNtpTime;
   /// inital RTP timestamp
   RtpTime seedRtpTime;
   /// rtp interval
   RtpTime sampleRate;
   /// payload specific sample rate
   int baseSampleRate;
   /// previous packet
   RtpPacket* prevPacket;

   /// number of packets received
   int packetReceived;
   /// number of bytes of payload received
   int payloadReceived;

   /// last packet RtpTime received 
   RtpTime prevPacketRtpTime;
   /// last RtpTime play
   RtpTime prevRtpTime;
   /// last NtpTime play
   NtpTime prevNtpTime;

   // previous sequence number received (after some processing)
   RtpSeqNumber prevSeqRecv;
   // We subtract sometimes, so make sure that a -1 is not promoted to more than
   // 16 bits of bits, since the RTP sequence number is really a 16-bit unsigned
   // number.
   void setPrevSeqRecv(RtpSeqNumber rsn) { prevSeqRecv = (rsn & 0xFFFF); }


   // Previous seq received.  Will not hide dups, reorder,
   // dropped sequenc numbers, etc.  Used for accounting
   // purposes only at this time.
   __uint16_t realPrevSeqRecv;

   /// previous sequence numer played
   RtpSeqNumber prevSeqPlay;

   // Various stats
   /// number of received sequence number cycles
   unsigned int recvCycles;
   /// number of played sequence number cycles
   unsigned int playCycles;

   unsigned int silencePatched; // How many times did we fill in silence?
   unsigned int insertedOooPkt; // How many times did we insert an out-of-order pkt?
   unsigned int comfortNoiseDropped; // Comfort noise dropped count.
   unsigned int packetBadlyMisordered; // So badly out of order we clear past history.
   unsigned int packetTooOld; //Have already played past this one, dropping it.
   unsigned int jitterBufferEmpty; // How many times was JB empty when we tried to read?

   /// relative transmission time for prev packet
   int lastTransit;

   /// estimated jitter, between when pkt arrived and when we
   // thought it should arrive.
   int jitter;

   /// jitter time in ms time
   int jitterTime;

   /// additional SDES and RTCP information
   RtcpReceiver* rtcpRecv;


   /// allow RTCP stack to probe for information
   friend class RtcpTransmitter;
   /// allow RTCP stack to probe for information
   friend class RtcpReceiver;

   /// format of payload for stack
   RtpPayloadType format;

   /// codecString in case of dynamic payload type
   char codecString[32];

   int sampleSize; /* Beware codecs with variable size encoding schemes,
                    * you CANNOT assume length of packet from this value
                    * since this is a logical size, and the payload may
                    * be compressed!  This value is the same units as rtp-time
                    */

   // number of bytes per sample  (typical 1 or 2, used for endian conversion)
   int perSampleSize;

   // payload specific sample rate
   int clockRate;

   /// silence packet template
   char* silenceCodec;

   /// my UDP stack
   Sptr<UdpStack> myStack;


   // Indexes into our Jitter Buffer.
   // When inPos == playPos, the buffer is 'empty'.
   unsigned int inPos;
   unsigned int playPos;

   void incrementInPos();
   void incrementPlayPos();
   unsigned int calculatePreviousInPos(int packets_ago) const;
};



#endif // RTPRECEIVER_HXX
