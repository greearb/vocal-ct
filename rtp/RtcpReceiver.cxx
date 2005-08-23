
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

static const char* const RtcpReceiver_cxx_Version =
    "$Id: RtcpReceiver.cxx,v 1.8 2005/08/23 06:39:42 greear Exp $";


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
#include <map>

// networking
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "cpLog.h"
#include "vsock.hxx"

#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "NtpTime.hxx"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include <misc.hxx>
#include <errno.h>


/* ----------------------------------------------------------------- */
/* --- rtpcReceiver Constructor ------------------------------------ */
/* ----------------------------------------------------------------- */

RtcpReceiver::RtcpReceiver (uint16 tos, uint32 priority,
                            const string& local_ip, const string& local_dev_to_bind_to,
                            int localMinPort, int localMaxPort) {
   myStack = new UdpStack(tos, priority, false, local_ip, local_dev_to_bind_to, NULL,
                          localMinPort, localMaxPort);

   constructRtcpReceiver();
}

RtcpReceiver::RtcpReceiver (uint16 tos, uint32 priority,
                            const string& local_ip, const string& local_dev_to_bind_to,
                            int localPort) {
   myStack = new UdpStack(tos, priority, false, local_ip, local_dev_to_bind_to,
                          NULL, localPort) ;

   constructRtcpReceiver();
}

RtcpReceiver::RtcpReceiver (Sptr<UdpStack> udp) {
   myStack = udp;
   constructRtcpReceiver();
}


void RtcpReceiver::constructRtcpReceiver () {
    packetReceived = 0;
    accumOneWayDelay = 0;
    avgOneWayDelay = 0;
    accumRoundTripDelay = 0;
    avgRoundTripDelay = 0;
}


RtcpReceiver::~RtcpReceiver () {
   // Nothing to do here
}


/* --- receive packet functions ------------------------------------ */
int RtcpReceiver::getPacket (RtcpPacket& pkt) {

   // receive packet
   //unsigned int fromLen = sizeof (txAddress);
   //int len = recvfrom (socketFD, p->getPacketData(), p->getPacketAlloc(),
   //                    0, (struct sockaddr*) &txAddress, &fromLen);
   //    int len = myStack->receive  (p->getPacketData(), p->getPacketAlloc());
   NetworkAddress sender("0.0.0.0");
   int len = myStack->receiveFrom (pkt.getPacketData(),
                                   pkt.getPacketAlloc(),
                                   &sender, MSG_DONTWAIT);
   //cpLog(LOG_DEBUG_STACK, "RTCP receiveFrom: %s : %d", sender.getIpName().c_str(),
   //          sender.getPort());
   
   // ? RtpTime arrival = generateTime ();
   if (len <= 0) {
      return len;
   }
   pkt.setTotalUsage (len);
   
   // check packet
   if (!isValid(&pkt)) {
      return -EINVAL;
   }
   return len;
}



int RtcpReceiver::isValid (RtcpPacket* p) {
   char* begin = reinterpret_cast < char* > (p->getPacketData());
   char* end = reinterpret_cast < char* > (begin + p->getTotalUsage());
   RtcpHeader* middle = reinterpret_cast < RtcpHeader* > (begin);

   // check if known payload type for first packet
   if (middle->type != rtcpTypeSR && middle->type != rtcpTypeRR) {
      return 0;
   }

   // check padbyte
   if (middle->padding) {
      return 0;
   }

   // check header lengths
   while (begin < end && (int)middle->version == RTP_VERSION) {
      begin += (ntohs(middle->length) + 1) * sizeof(RtpSrc);
      middle = reinterpret_cast < RtcpHeader* > (begin);
   }

   if (begin != end) {
        return 0;
   }

   // exit with success
   cpLog(LOG_DEBUG_STACK, "RTCP packet is valid");
   //    cout << "RTCP packet is valid" << endl;
   return 1;
}



int RtcpReceiver::readRTCP (RtcpPacket* p) {

   char* begin = reinterpret_cast < char* > (p->getPacketData());
   char* end = reinterpret_cast < char* > (begin + p->getTotalUsage());
   RtcpHeader* middle = NULL;
   int ret = 0;

   while (begin < end) {
      middle = reinterpret_cast < RtcpHeader* > (begin);
      switch (middle->type) {
      case (rtcpTypeSR):
      case (rtcpTypeRR):
         readSR (middle);
         break;
      case (rtcpTypeSDES):
         readSDES (middle);
         break;
      case (rtcpTypeBYE):
         if ( readBYE (middle) == 0) {
            ret = 1;
         }
         break;
      case (rtcpTypeAPP):
         readAPP (middle);
         break;
      default:
         cpLog (LOG_ERR, "RTCP: Unknown RTCP type");
         break;
      }
      begin += (ntohs(middle->length) + 1) * sizeof(u_int32_t);
   }
   return ret;
}



RtcpHeader* RtcpReceiver::findRTCP (RtcpPacket* p, RtcpType type) {
   char* begin = reinterpret_cast < char* > (p->getPacketData());
   char* end = reinterpret_cast < char* > (begin + p->getTotalUsage());
   RtcpHeader* middle = NULL;

   while (begin < end) {
      middle = reinterpret_cast < RtcpHeader* > (begin);
      if (type == static_cast < RtcpType > (middle->type))
         return middle;
      begin += (ntohs(middle->length) + 1) * sizeof(u_int32_t);
   }

   // packet type not found
   cpLog (LOG_ERR, "RTCP: Type found here: %d", (int)type);
   return NULL;
}



/* --- Read SR RTCP packet ----------------------------------------- */
int RtcpReceiver::readSR (RtcpPacket* p) {
   RtcpHeader* head = findRTCP (p, rtcpTypeSR);
   if (head == NULL)
      head = findRTCP (p, rtcpTypeRR);
   if (head == NULL)
      return -1;

   readSR (head);

   // read next RR packet if found
   // future: - ?
   return 0;
}


void RtcpReceiver::readSR (RtcpHeader* head) {
   char* middle = NULL;

   NtpTime nowNtp = getNtpTime();
   
   // read SR block
   if (head->type == rtcpTypeSR) {
      RtcpSender* senderBlock = reinterpret_cast < RtcpSender* >
         ((char*)head + sizeof(RtcpHeader));
      Sptr<RtpTranInfo> s = findTranInfo(ntohl(senderBlock->ssrc));
      
      uint32 nts = ntohl(senderBlock->ntpTimeSec);
      uint32 ntf = ntohl(senderBlock->ntpTimeFrac);

      s->lastSRTimestamp = (nts << 16) | ((ntf >> 16) & 0xFFFF);
      s->recvLastSRTimestamp = nowNtp;

      //printSR (senderBlock);  // - debug

      packetReceived++;

      NtpTime thenNtp (nts, ntf);

      uint64 now = nowNtp.getMs();
      uint64 then = thenNtp.getMs();
      int diff = now - then;
      if (now < then) {
         diff = then - now;
         diff = -diff;
      }

      cpLog(LOG_DEBUG_STACK, "diff: %d  now: %llu  then: %llu  lastSRTimestamp: 0x%x  ntpTime: %x:%x\n",
            diff, now, then, s->lastSRTimestamp, nts, ntf);

      accumOneWayDelay += diff;

#ifdef USE_LANFORGE
      if (rtpStatsCallbacks) {
         rtpStatsCallbacks->avgNewOneWayLatency(now, diff);
      }
#endif

      avgOneWayDelay = accumOneWayDelay / packetReceived;
      
      middle = (char*)head + sizeof(RtcpHeader) + sizeof(RtcpSender);
   }
   else {
      // move over blank SR header
      middle = (char*)head + sizeof(RtcpHeader);
      
      // move over the ssrc of packet sender
      RtpSrc* sender = reinterpret_cast < RtpSrc* > (middle);
      RtpSrc ssrc;
      
      ssrc = ntohl(*sender);
      middle += sizeof(RtpSrc);
      
      packetReceived++;
   }
   

   // read RR blocks
   RtcpReport* block = reinterpret_cast < RtcpReport* > (middle);
   for (int i = head->count; i > 0; i--) {
      //printRR (block);  // - debug
      
      // Stamp for last packet received by the reporting endpoint.
      uint32 srt = ntohl(block->lastSRTimeStamp);
      NtpTime thenNtp ((srt >> 16) & 0xFFFF, srt << 16);
      
      NtpTime nowNtp1 (nowNtp.getSeconds() & 0x0000FFFF, 
                       (nowNtp.getFractional() & 0xFFFF0000 ));
      uint64 nowN = nowNtp1.getMs();
      uint64 thenN = thenNtp.getMs();
      
      int diff = nowN - thenN;
      if (nowN < thenN) {
         diff = thenN - nowN;
         diff = -diff;
      }
      
      unsigned int ld = ntohl(block->lastSRDelay);
      
      cpLog(LOG_DEBUG_STACK, "nowN: %llu  thenN: %llu  diff: %d  lastDelay: %d, block->lastSRTimeStamp: %x\n",
            nowN, thenN, diff, ld, srt);
      
      if (thenN != 0) {
         // Ignore this one if thenN is zero, we can't make effective decision
         // yet, as the other side
         // has not received anything from us yet.
         // Convert from 1/65536 second units to miliseconds.
         diff -= (ld / 65);
         
         accumRoundTripDelay += diff;
         
#ifdef USE_LANFORGE
         if (rtpStatsCallbacks) {
            
            if ((diff < -1000) || (diff > 1000)) {
               cpLog(LOG_ERR, "WARNING: rt_latency abnormal: nowN: %llu  thenN: %llu  diff: %d  lastDelay: %d, block->lastSRTimeStamp: %x\n",
                     nowN, thenN, diff, ld, srt);
            }
            uint64 nw = nowNtp.getMs();
            cpLog(LOG_DEBUG_STACK, "NOTE:  Inserting lanforge latency: %d, nw: %llu\n",
                  diff, nw);
            rtpStatsCallbacks->avgNewRoundTripLatency(nw, diff);
         }
#endif
         // NOTE:  This can't really be right, since packetReceived is not
         // incremented inside the loop.  For LANforge, ignoring it all together.
         // --Ben
         avgRoundTripDelay = accumRoundTripDelay / packetReceived;
      }

      ++block;
   }

    // handle profile specific extensions
    // - ?
}




void RtcpReceiver::printSR (RtcpSender* p) {
    cerr << "Got SR from " << ntohl(p->ssrc) << endl;
    cerr << "  NTP time: " << ntohl(p->ntpTimeSec) << " ";
    cerr << ntohl(p->ntpTimeFrac) << endl;
    cerr << "  RTP time: " << ntohl(p->rtpTime) << endl;
    cerr << "  Packets sent: " << ntohl(p->packetCount);
    cerr << "    Payload sent: " << ntohl(p->octetCount) << endl;
}


void RtcpReceiver::printRR (RtcpReport* p) {
    cerr << "Got RR for " << ntohl(p->ssrc) << endl;
    cerr << "  Lost Frac: " << p->fracLost;
    u_int32_t lost = (p->cumLost[0] << 16) | (p->cumLost[1] << 8) | p->cumLost[2];
    lost = lost & (0xffffff);
    cerr << "  Lost count: " << lost;
    cerr << "  Cycles: " << ntohs(p->recvCycles);
    cerr << "  Last seq: " << ntohs(p->lastSeqRecv) << endl;
    cerr << "  Jitter: " << ntohl(p->jitter) << "  ";
    cerr << "Last SR: " << ntohl(p->lastSRDelay) << endl;
}



/* --- Read SDES packet -------------------------------------------- */

int RtcpReceiver::readSDES (RtcpPacket* p) {
   RtcpHeader* head = findRTCP (p, rtcpTypeSDES);
   if (head == NULL)
      return -1;

   readSDES (head);

   // read next SDES packet if found
   // future: - ?
   return 0;
}


void RtcpReceiver::readSDES (RtcpHeader* head) {
   char* begin = reinterpret_cast < char* > ((char*)head + sizeof(RtcpHeader));
   RtcpChunk* middle = reinterpret_cast < RtcpChunk* > (begin);

   RtcpSDESItem* item = NULL;
   RtcpSDESItem* nextitem = NULL;
   RtpSrc ssrc;
   
   for (int i = head->count; i > 0; i--) {
      ssrc = ntohl(middle->ssrc);
      
      for (item = &(middle->startOfItem); item->type; item = nextitem) {
         addSDESItem(ssrc, item);
         nextitem = reinterpret_cast < RtcpSDESItem* >
            ((char*)item + sizeof(RtcpSDESItem) - 1 + item->length);
      }
      
      middle = reinterpret_cast < RtcpChunk* > (item);
   }
}



void RtcpReceiver::addSDESItem (RtpSrc src, RtcpSDESItem* item) {
   Sptr<RtpTranInfo> s = findTranInfo(src);
   
   switch (item->type) {
   case rtcpSdesCname:
      strncpy ((s->SDESInfo).cname, &(item->startOfText), item->length + 1);
      break;
   case rtcpSdesName:
      strncpy ((s->SDESInfo).name, &(item->startOfText), item->length + 1);
      break;
   case rtcpSdesEmail:
      strncpy ((s->SDESInfo).email, &(item->startOfText), item->length + 1);
      break;
   case rtcpSdesPhone:
      strncpy ((s->SDESInfo).phone, &(item->startOfText), item->length + 1);
      break;
   case rtcpSdesLoc:
      strncpy ((s->SDESInfo).loc, &(item->startOfText), item->length + 1);
      break;
   case rtcpSdesTool:
      strncpy ((s->SDESInfo).tool, &(item->startOfText), item->length + 1);
      break;
   case rtcpSdesNote:
      strncpy ((s->SDESInfo).note, &(item->startOfText), item->length + 1);
      break;
   case rtcpSdesPriv:
      // future: not implemented
   default:
      cpLog (LOG_ERR, "RtcpReceiver: SDES type unknown");
      break;
   }

   /*
   // - debug
   cerr <<"Update "<<src<<" with "<< (int) item->type <<" "<< (int) item->length;
   char output [255];
   memset (output, 0, 255);
   strncpy (output, &(item->startOfText), item->length+1);
   cerr << endl <<output<<endl;
   cerr <<"_SDES_";
   */
}



/* --- Read BYE packet --------------------------------------------- */

int RtcpReceiver::readBYE (RtcpPacket* p) {
   RtcpHeader* head = findRTCP (p, rtcpTypeBYE);
   if (head == NULL)
      return -1;
   
   readBYE (head);

   // read next BYE packet if found
   // future: - ?
   return 0;
}


int RtcpReceiver::readBYE (RtcpHeader* head) {

   //    char* end = reinterpret_cast<char*>
   //                ((char*)head + sizeof(RtpSrc) * (ntohs(head->length) + 1));
   RtpSrc* src = reinterpret_cast < RtpSrc* >
      ((char*)head + sizeof(RtcpHeader));
   
   
   for (int i = head->count; i > 0; i--) {
      cpLog( LOG_DEBUG_STACK, "readRtcpBye for %d", ntohl(*src) );
      //       cerr << "readRtcpBye for " << ntohl(*src) << endl;
      removeTranInfo (ntohl(*src++));
   }
   
   return 0;

   //TODO Convert this to cpLog if necessary
#if 0
   // print reason
   char* middle = reinterpret_cast < char* > (src);
   //    if (middle != end)
   {
      RtcpBye* reason = reinterpret_cast < RtcpBye* > (middle);
      cerr << "   Reason: ";    // - debug
      cerr.write(&(reason->startOfText), (int) reason->length);    // - debug
      cerr << endl;    // - debug
   }
   //cerr <<"_BYE_";  // - debug
   
#endif
}



/* --- Read APP packet --------------------------------------------- */

int RtcpReceiver::readAPP (RtcpPacket* p) {
   RtcpHeader* head = findRTCP (p, rtcpTypeAPP);
   if (head == NULL)
      return -1;
   
   readAPP (head);

   // read next APP packet if found
   // future: - ?
   return 0;
}


void RtcpReceiver::readAPP (RtcpHeader* head) {
   // future: not implemented
   assert (0);
}



/* --- known transmitter list functions ---------------------------- */

Sptr<RtpTranInfo> RtcpReceiver::addTranInfo (RtpSrc src, RtpReceiver* recv) {
   
   //    cout << "adding: " << src << endl;
   
   if (recv)
      assert (src == recv->ssrc);

   Sptr<RtpTranInfo> s = new RtpTranInfo();
   s->recv = recv;
   s->ssrc = src;
   s->expectedPrior = 0;
   s->receivedPrior = 0;
   s->lastSRTimestamp = 0;

   //    cout << "adding ptr: " << s << endl;

   if (addTranFinal (s)) {
      s = findTranInfo (src);
      assert (s->recv == NULL);  // - ?
      s->recv = recv;
   }
   return s;
}


int RtcpReceiver::addTranFinal (Sptr<RtpTranInfo> s) {
   // add transmitter to listing
   pair < map < RtpSrc, Sptr<RtpTranInfo> > ::iterator, bool > result =
      tranInfoList.insert (pair < RtpSrc, Sptr<RtpTranInfo> >
                           (s->ssrc, s));

   if (!result.second) {
      //transmitter already in listing
      return 1;
   }
   //cpLog (LOG_DEBUG_STACK, "RTCP: Transmitter add: %d", s->ssrc);
   return 0;
}



int RtcpReceiver::removeTranInfo (RtpSrc src, int flag)
{
    //    cout << "RTCP: removing: " << src << endl;
   map < RtpSrc, Sptr<RtpTranInfo> > ::iterator p = tranInfoList.find(src);
   if (p == tranInfoList.end()) {
      // src not found
      assert (0);
   }

   Sptr<RtpTranInfo> info = p->second;

   //    cout << "RTCP: removing ptr: " << info << endl;

   // remove from RTP stack
   if (info->recv && !flag)
      (info->recv)->removeSource(info->ssrc, 1);
   info->recv = NULL;

   // remove from receiver list
   tranInfoList.erase (p);

   //cpLog (LOG_DEBUG_STACK, "RTCP: Transmitter removed: %d", src);
   return 0;
}



Sptr<RtpTranInfo> RtcpReceiver::findTranInfo (RtpSrc src) {
   Sptr<RtpTranInfo> info;

   map < RtpSrc, Sptr<RtpTranInfo> > ::iterator p = tranInfoList.find(src);

   if (p == tranInfoList.end())
      // receiver not found, so add it
      info = addTranInfo(src);
   else
      info = p->second;

   return info;
}


Sptr<RtpTranInfo> RtcpReceiver::getTranInfoList (int index) {
   assert (index >= 0);
   assert (index < getTranInfoCount());

   map < RtpSrc, Sptr<RtpTranInfo> > ::iterator p = tranInfoList.begin();
   for (int i = 0; i < index; i++) {
      ++p;
   }

   assert (p != tranInfoList.end());

   return p->second;
}


int RtcpReceiver::getTranInfoCount() {
   return tranInfoList.size();
}



int RtcpReceiver::getPort () {
    return myStack->getRxPort();
};


int RtcpReceiver::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                         int& maxdesc, uint64& timeout, uint64 now) {
   myStack->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   return 0;
}
