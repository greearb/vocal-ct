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

static const char* const SipUdpConnection_hxx_Version =
    "$Id: SipUdpConnection.hxx,v 1.3 2004/05/27 04:32:18 greear Exp $";



#include "Sptr.hxx"
#include "SipMsg.hxx"
#include "SipTransactionId.hxx"
#include "TransceiverSymbols.hxx"
#include <misc.hxx>
#include "SipTransactionLevels.hxx"
#include <list>
#include <queue>
#include <UdpStack.hxx>
#include "RetransmitContents.hxx"

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
        SipUdpConnection(const string& local_ip,
                         const string& local_dev_to_bind_to,
                         int port = SIP_PORT);
        
        virtual ~SipUdpConnection();
        
        void send(Sptr<SipMsgContainer> msg, const Data& host,
                  const Data& port);

        int udpSend(Sptr<SipMsgContainer> msg);

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

        static bool Udpretransmitoff;
        static int  Udpretransmitimeinitial;
        static int Udpretransmittimemax;
        int randomLosePercent;

        UdpStack udpStack;
        priority_queue <Sptr <RetransmitContents> > sendQ;
        list <Sptr <SipMsgContainer> > rcvFifo;
        char rcvBuf[MAX_UDP_RCV_BUF];

        static atomic_t _cnt;
};
 
} // namespace Vocal

#endif
