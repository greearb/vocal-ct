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
   bool is_in_use; //Ie, does this sample logically exist?
   uint32 rtp_seq_no;
   uint32 rtp_time;

public:
   RtpData() : len(0), is_silence_fill(false),
               is_in_use(false),
               rtp_seq_no(0), rtp_time(0) { }

   void setData(const char* incomming_data, int data_len) {
      assert(data_len <= MAX_RTP_DATA);
      memcpy(data, incomming_data, data_len);
      len = data_len;
   }
   void setSilenceFill(bool b) { is_silence_fill = b; }
   void setIsInUse(bool b) { is_in_use = b; }
   void setRtpSequence(uint32 s) { rtp_seq_no = s; }
   void setRtpTime(uint32 s) { rtp_time = s; }

   bool isInUse() const { return is_in_use; }
   bool isSilenceFill() const { return is_silence_fill; }

   uint32 getRtpSequence() const { return rtp_seq_no; }
   uint32 getRtpTime() const { return rtp_time; }

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
   RtpReceiver (const char* dbg_msg, uint16 tos, uint32 priority, const string& local_ip,
                const string& local_dev_to_bind_to,
                int localPort,
                RtpPayloadType _format, int _clockrate, int per_sample_size,
                int samplesize, int jitter_buffer_sz);

   /** using port range
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    */
   RtpReceiver (const char* dbg_msg, uint16 tos, uint32 priority, const string& local_ip,
                const string& local_dev_to_bind_to,
                int localMinPort, int localMaxPort,
                RtpPayloadType _format, int _clockrate, int per_sample_size,
                int samplesize, int jitter_buffer_sz);

   /** Giving UdpStack ptr */
   RtpReceiver (Sptr<UdpStack> udp, RtpPayloadType _format,
                int _clockrate, int per_sample_size,
                int samplesize, int jitter_buffer_sz );

   /// consturctor init (don't call this function)
   void constructRtpReceiver (RtpPayloadType _format,
                              int clockrate, int per_sample_size,
                              int samplesize, int jitter_buffer_sz);
   ///
   virtual ~RtpReceiver ();


   /** Receive an RTP packet from the network.
   **/
   int readNetwork();

   /**
      @return <= 0 implies one should not inspect pkt, nothing worth receiving
      was received.
   */
   int retrieve(RtpPacket& pkt, const char* dbg);

   /** Helper method to find the right silence fill. */
   //void resolveSilencePacket(bool& faking);

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
   void clearJitterBuffer();

   /// Prints out the jitter buffer.
   void printBuffer(int log_level);

   /// Empties pending network packets
   void emptyNetwork ();

   ///
   void setRTCPrecv (RtcpReceiver* rtcpRecv);

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
   int getJitter () const { return (jitter >> 4); }

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
   void verifyJbSanity(const char* dbg);

   uint16 getPrevSeqRecv() { return realPrevSeqRecv; }
   uint32 getRecvCycles() { return recv_cycles; }

protected:

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

   // Previous seq received.  Will not hide dups, reorder,
   // dropped sequenc numbers, etc.  Used for accounting
   // purposes only at this time.
   uint16 realPrevSeqRecv;
   uint32 recv_cycles;

   // Various stats
   unsigned int comfortNoiseDropped; // Comfort noise dropped count.

   /// relative transmission time for prev packet
   uint64 lastTransit_ms;

   /// estimated jitter, between when pkt arrived and when we
   // thought it should arrive.  Units are 'ms' << 4
   int jitter;

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

   /// my UDP stack
   Sptr<UdpStack> myStack;

   // Cache this object here so we don't have to create & destroy it on
   // the stack.
   RtpPacket tmpPkt;

   // Indexes into our Jitter Buffer.
   // If inPos == playPos, then it is EITHER empty or Full.
   //  If the buffer at [inPos] is in use, then it is full, else empty.
   unsigned int inPos; // Location to insert next.
   unsigned int playPos; // Location to play next.

   void incrementInPos();
   void incrementPlayPos(const char* dbg);
   unsigned int calculatePreviousInPos(int packets_ago) const;
   int insertSilenceRtpData(uint32 rtp_time, uint16 rtp_seq);
   int appendRtpData(RtpPacket& pkt);
   int setRtpData(RtpPacket& pkt, int idx);
   unsigned int cur_max_jbs; // How big (logically) is our jitter buffer
   RtpData* jitterBuffer[JITTER_BUFFER_MAX];
   bool readRealVoiceAlready;

   RtpData* getLastRtpData();
};



#endif // RTPRECEIVER_HXX
