
#ifndef RTCPRECEIVER_HXX
#define RTCPRECEIVER_HXX

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000,2001, 2003 Vovida Networks, Inc.  All rights reserved.
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

static const char* const RtcpReceiver_hxx_Version =
    "$Id: RtcpReceiver.hxx,v 1.3 2004/06/15 06:20:35 greear Exp $";


#include "Rtcp.hxx"

#include <sys/types.h>
#include <map>

#include "UdpStack.hxx"
#include <BugCatcher.hxx>
#include "rtpTypes.h"
#include "NetworkAddress.h"


/** Class to receive RTCP packets.
    @see RtpSession
 */
class RtcpReceiver : public BugCatcher {
public:
   /** Constructor creating incoming RTCP stack using port ranges
       @param port associated port
       @param local_ip  Local IP to bind to, use "" for system default.
       * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
       **/
   RtcpReceiver (const string& local_ip, const string& local_dev_to_bind_to,
                 int localMinPort, int localMaxPort);

   /** Constructor creating incoming RTCP stack using specified port
       @param port associated port
       @param local_ip  Local IP to bind to, use "" for system default.
       * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
       **/
   RtcpReceiver (const string& local_ip, const string& local_dev_to_bind_to,
                 int localPort);
   /** Constructor, using udp stack ptr **/
   RtcpReceiver (Sptr<UdpStack> udp);

   /// consturctor init (don't call this function)
   void constructRtcpReceiver ();
   ///
   virtual ~RtcpReceiver ();


   /** Receives an incoming RTCP packet
    *  @return: <= 0 means failed to read packet.
    **/
   int getPacket (RtcpPacket& pkt);

   /** Checks if an RTCP packet is valid
    *  @return 0 not valid, 1 valid
    **/
   int isValid (RtcpPacket* packet);

   /// reads compound RtcpPacket and calls apporiate read function
   int readRTCP (RtcpPacket* packet);

   /** searches inside packet for given type.
       @return NULL means type not found, else ptr to first found
   **/
   RtcpHeader* findRTCP (RtcpPacket* packet, RtcpType type);


   /**
    *  These functions will search inside packet for apporiate type
    *  @return -1 if type not found.  Otherwise will call related
    *  function to use packet and return 0.
    **/
   int readSR (RtcpPacket* packet);
   ///
   int readSDES (RtcpPacket* packet);
   ///
   int readBYE (RtcpPacket* packet);
   ///
   int readAPP (RtcpPacket* packet);
   
   /**
    *  These functions will search inside packet for apporiate type
    * Thses functions will read packet and store packet information.
    * Same result as calling, ie  readSR(findRTCP(p, RtcpTypeSR))
    **/
   void readSR (RtcpHeader* head);
   ///
   void readSDES (RtcpHeader* head);
   ///
   int readBYE (RtcpHeader* head);
   ///
   void readAPP (RtcpHeader* head);


   ///
   void printSR (RtcpSender* p);
   ///
   void printRR (RtcpReport* p);
   ///
   void addSDESItem (RtpSrc src, RtcpSDESItem* item);


   /// Adds receiver to source listing
   Sptr<RtpTranInfo> addTranInfo (RtpSrc src, RtpReceiver* recv = NULL);
   int addTranFinal (Sptr<RtpTranInfo> s);

   /** Remove receiver from  source listing
    *  @return 0 sucess, 1 not found
    **/
   int removeTranInfo (RtpSrc src, int flag = 0);

   /** Finds pointer to source structure
    *  Creates source struture if not found
    *  @return pointer to source structure
    **/
   Sptr<RtpTranInfo> findTranInfo (RtpSrc src);
   
   /// Access specified souurce infomration
   Sptr<RtpTranInfo> getTranInfoList (int index);
   
   /// Number of known sources
   int getTranInfoCount ();
   
   /// Port this receiver is receiving it signal
   int getPort ();
   /// Socket File Descriptor used for select()
   int getSocketFD ();
   
   /// get the ptr of my UdpStack
   Sptr<UdpStack> getUdpStack() {
      return myStack;
   }

   /// get the data for latency
   int getAvgOneWayDelay() {
      return avgOneWayDelay;
   }
   int getAvgRoundTripDelay() {
      return avgRoundTripDelay;
   }

private:

   map < RtpSrc, Sptr<RtpTranInfo> > tranInfoList;

   /// my UDP stack
   Sptr<UdpStack> myStack;

   ///
   int packetReceived;

   ///
   int accumOneWayDelay;
   int avgOneWayDelay;

   ///
   int accumRoundTripDelay;
   int avgRoundTripDelay;
};


#endif // RTCPRECEIVER_HXX
