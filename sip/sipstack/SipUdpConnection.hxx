#ifndef SIP_UDP_CONNECTION_HXX
#define SIP_UDP_CONNECTION_HXX

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



#include "Sptr.hxx"
#include "SipMsg.hxx"
#include "SipTransactionId.hxx"
#include "TransceiverSymbols.hxx"
#include <misc.hxx>
#include "SipMsgContainer.hxx"
#include <list>
#include <queue>
#include <UdpStack.hxx>

#define MAX_UDP_RCV_BUF 65536

namespace Vocal
{
    
class SipUdp_impl;
class SipMsgContainer;

///
class SipUdpConnection: public BugCatcher {
public:
   /**
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    */
   SipUdpConnection(const char* debug_msg, uint16 tos, uint32 priority,
                    const string& local_ip,
                    const string& local_dev_to_bind_to,
                    int port = SIP_PORT);
   
   virtual ~SipUdpConnection();
   
   int send(Sptr<SipMsgContainer> msg, const Data& host,
            const Data& port);
   
   int udpSend(Sptr<SipMsgContainer> msg);

   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);

   
   // Returns the configured IP, if it is not "", otherwise, returns
   // default system IP address.
   const string getLocalIp() const;
   
   static void reTransOn();
   
   static void reTransOff();
   
   static void setRetransTime(int initial = retransmitTimeInitial,
                              int max = retransmitTimeMax /* default values from TransceiverSymbols.hxx */);
   
   void setRandomLosePercent(int percent);
   
   void printSize() const;
   
   Data getDetails() const;
   
   
   int receiveMain();

   // May pull from incomming fifo
   Sptr<SipMsgContainer> getNextMessage();
   
   int sendMain(uint64& now);
   
   void getHostPort(Sptr<SipMsg> sipMessage, Data& host, int& port);
   

   static int getInstanceCount() { return atomic_read(&_cnt); }

protected:
   // Read from socket.
   Sptr<SipMsgContainer> receiveMessage();
   
private:
   ///
   SipUdpConnection();
   SipUdpConnection(const SipUdpConnection& src);
   SipUdpConnection& operator = (const SipUdpConnection& src) const;
   
   //static bool Udpretransmitoff;
   static int  Udpretransmitimeinitial;
   static int Udpretransmittimemax;
   int randomLosePercent;
   char rcvBuf[MAX_UDP_RCV_BUF];
   
   UdpStack udpStack;
   priority_queue <Sptr <SipMsgContainer>,
                   vector< Sptr<SipMsgContainer> >,
                   SipMsgContainerComparitor > sendQ;
   list < Sptr <SipMsgContainer> > rcvFifo;
   
   static atomic_t _cnt;
};
 
} // namespace Vocal

#endif
