#ifndef RTPSESSION_HXX
#define RTPSESSION_HXX

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

static const char* const RtpSession_hxx_Version =
    "$Id: RtpSession.hxx,v 1.4 2004/10/29 07:22:34 greear Exp $";



#include "rtpTypes.h"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "DTMFInterface.hxx"

/**

   Contains everything needed for a complete RTP Session, including
   RTP/RTCP transmitter and RTP/RTCP receiver.<P>

   <b>To construct a stack:</b><p>

   <pre>
   int remotePort = 10000;
   int localPort  = 10000;
   int remoteRtcpPort = 10001;
   int localRtcpPort =  10001;
   pType = rtpPayloadPCMU;
   RtpSession&#42; rtpStack = new RtpSession ( "127.0.0.1", remotePort, localPort,
                                           remoteRtcpPort, localRtcpPort, 
                                           pType,
                                           pType, 0 );
   </pre>

   <p>
   <b>To change the send/receive state of the stack:</b><p>
   
   <pre>
   rtpStack->setSessionState(rtp_session_recvonly);
   rtpStack->setSessionState(rtp_session_sendonly);
   rtpStack->setSessionState(rtp_session_sendrecv);
   </pre>
   <p>
   <b>To receive RTP:</b><P>

   <pre>
   while(1)
   {
       RtpSessionState sessionState = rtpStack->getSessionState();

       if( (sessionState == rtp_session_recvonly || 
            sessionState == rtp_session_sendrecv))
       {
            RtpPacket&#42; packet = rtpStack->receive();
            
            ... // do something here.
            
            // at this point, the packet data (in encoded form) is
            // available via the following interfaces:
            
            // packet.getPayloadLoc() contains a ptr to the start of
            // the packet.
            
            // packet.getPayloadUsage() returns the # of valid bytes
            // pointed to by getPayloadLoc().
       }
   }
   </pre>
   <p>

   <b>In order to send packets:</b><p>
   <P>
   <pre>
   char&#42; data;
   int length;
   
   // at this point, assume data points to the raw audio data to send
   // and that length is the # of valid bytes.
   
   rtpStack->transmitRaw(data, length);
   </pre>

   
*/
class RtpSession {
public:
   /**@param ports All ports are optional
    * @param local_ip  Local IP to bind to, use "" for system default.
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    **/
   RtpSession (const string& local_ip,
               const string& local_dev_to_bind_to,
               const char* remoteHost, int remotePort, int localPort,
               int rtcpRemotePort, int rtcpLocalPort,
               RtpPayloadType format, int clockrate, int per_sample_size,
               int samplesize);

   /**@param ports All ports are optional by passing 0.  Port range is
    *        availiable here.
    * @param local_ip  Local IP to bind to, use "" for system default.
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    **/
   RtpSession (const string& local_ip,
               const string& local_dev_to_bind_to,
               const char* remoteHost, int remotePort, int localPort,
               int rtcpRemotePort, int rtcpLocalPort, int portRange,
               RtpPayloadType format, int clockrate, int per_sample_size,
               int samplesize);

   /** consturctor init (don't call this function)
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    */
   void constructRtpSession (const string& local_ip,
                             const string& local_dev_to_bind_to,
                             const char* remoteHost, int remotePort,
                             int localPort, int rtcpRemotePort, int rtcpLocalPort,
                             int portRange, RtpPayloadType format,
                             int clockrate, int per_sample_size,
                             int samplesize);
   ///
   virtual ~RtpSession ();
        
   /**  reserves a rtp port by instantiating a RtpReceiver object
    * @param local_ip  Local IP to bind to, use "" for system default.
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    */
   int reserveRtpPort(const string& local_ip,
                      const string& local_dev_to_bind_to,
                      int localMin = -1, int localMax = -1);
   /// release the rtp port
   int releaseRtpPort();

   /** reserves a rtcp port by instantiating a RtcpReceiver object
    * @param local_ip  Local IP to bind to, use "" for system default.
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    */
   int reserveRtcpPort(const string& local_ip, const string& local_dev_to_bind_to,
                       int localPort, int portRange = 0);
   /// release the rtcp port
   int releaseRtcpPort();

   /**@name Session Functions
    *  Being in a particular state disables some functions.  For example,
    *  in recvonly state, the transmit function will do nothing.
    *  Availiable state are listed in rtpTyes.h
    **/
   void setSessionState (RtpSessionState state);
   ///
   RtpSessionState getSessionState ();

   /** set receivers
    * @param local_ip  Local IP to bind to, use "" for system default.
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    */
   int setReceiver ( const string& local_ip,
                     const string& local_dev_to_bind_to,
                     int localMinPort, int rtcpLocalPort, int portRange,
                     RtpPayloadType format, int clockrate, int per_sample_size,
                     int samplesize);

   /** set transmitters
    * @param local_ip  Local IP to bind to, use "" for system default.
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    */
   int setTransmiter ( const string& local_ip,
                       const string& local_dev_to_bind_to,
                       const char* remoteHost, int remotePort,
                       int rtcpRemotePort, RtpPayloadType format,
                       int clockrate, int per_sample_size, int samplesize);

   ///
   void setFormat (RtpPayloadType type, int clockrate, int per_sample_size,
                   int samplesize);

   ///
   void setCodecString (const char* codecStringInput);

   /// size is in number of samples
   void setSampleSize (int size);

   ///
   int getSampleSize ();

   ///
   int getPacketSent ();
   ///
   int getByteSent ();

   ///
   int getPacketReceived ();
   ///
   int getByteReceived ();

   ///
   int getPacketLost ();

   ///
   int getJitter ();

   ///
   int getLatency ();

   // not implemented
   //void setTypeOfService (int service);
   // not implemented
   //int getTypeOfService ();
   //@}


   /** Creates an RTP packet for transmitter
    *  @param no_samples number of samples for specified payload
    *  @param padbyteSize always 0, not fully implemented
    *  @param csrc_count number of contributing sources to packet
    **/
   RtpPacket* createPacket (int padbyteSize = 0, int csrcCount = 0);

   /** Gets the previous packet's RTP sequence number 
    **/
   RtpSeqNumber getPrevSequence();

   /** Gets the previous packet's RTP timestamp
    **/
   RtpTime getPrevRtpTime();

   // set marker flago next packet
   void setMarkerOnce();

   /** Transmits packet onto network
    *  @return -1 error, otherwise size sent on sucess
    **/
   int transmit (RtpPacket& packet);
   int transmitRaw (char* inbuffer, int len);

   /** Transmit DTMF event in RTP on network
    *  @return -1 error, otherwise 0
    **/
   int transmitEvent( int event );

   /** Receives RTP packet information from network
    * Returns <= 0 if packet is not valid.
    **/
   int readNetwork(fd_set* fds);

   // Read from the jitter buffer.
   int retrieve(RtpPacket& pkt);


   /** Checks RTCP interval, transmits and receeives if neccessary
    **/
   void processRTCP();

   /** Create and transmit compound RTCP packet
    *  @return -1 failure, else number of bytes sent on success
    **/
   int transmitRTCP ();

   /** Create and transmit a SR and BYE compound packet
    *  @return -1 failure, else number of bytes sent on success
    **/
   int transmitRTCPBYE ();

   /** receive and read compound RTCP packet
    *  @eturn 0 if no packet received, else 1 on sucess
    **/
   int receiveRTCP ();

   /** Check if time to send RTCP packet
    *  return: 1 = time to send RTCP packet
    **/
   int checkIntervalRTCP ();


   /// RTP transmitter
   RtpTransmitter* getRtpTran () {
      return tran;
   }

   /// RTP recevier
   RtpReceiver* getRtpRecv () {
      return recv;
   }

   /// RTCP transmitter
   RtcpTransmitter* getRtcpTran () {
      return rtcpTran;
   }

   /// RTCP receiver
   RtcpReceiver* getRtcpRecv () {
      return rtcpRecv;
   }

   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);

   //Current number of elements in jitter buffer.
   unsigned int getJitterPktsInQueueCount() const {
      return recv->getJitterPktsInQueueCount();
   }

   unsigned int getCurMaxPktsInQueue() const {
      return recv->getCurMaxPktsInQueue();
   }

   /// Rtp event class
   RtpEvent _rtpEvent;

   /// DTMF callback function
   void setDTMFInterface ( DTMFInterface* t );
   void unsetDTMFInterface ( DTMFInterface* t );

private:
   /// Internal pointer to RTP transmitter
   RtpTransmitter* tran;
   /// Internal pointer to RTP receiver
   RtpReceiver* recv;
   /// Internal pointer to RTCP transmitter
   RtcpTransmitter* rtcpTran;
   /// Internal pointer to RTCP receiver
   RtcpReceiver* rtcpRecv;

   /// session state
   RtpSessionState sessionState;

   /// session error code;
   RtpSessionError sessionError;

   //Restricted.
   RtpSession();
   RtpSession(const RtpSession& rhs);
   RtpSession& operator=(const RtpSession& rhs);
};



#endif // RTPSESSION_HXX
