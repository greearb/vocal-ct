#ifndef SipUdp_impl_HXX
#define SipUdp_impl_HXX
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

static const char* const SipUdp_impl_hxx_Version = 
"$Id: SipUdp_impl.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "Sptr.hxx"
#include "SipTransactionStatus.hxx"
#include "VThread.hxx"
#include "UdpStack.hxx"
#include "Fifo.h"
#include "TransceiverSymbols.hxx"  
#include "RetransmitContents.hxx"

#ifdef USE_VFILTER
#include "VFilter.hxx"  
#endif

#include "SipTransactionLevels.hxx"
#include "SipTransactionGC.hxx"


namespace Vocal
{
    
///
class SipUdp_impl
{
    public:

        ///  local_ip may be "", will use default system address in that
        // case (bind to all, as well).
        // If local_interface_to_bind_to is not "", then we will attempt to bind
        // to it with SO_BINDTODEVICE.
        SipUdp_impl(Fifo<SipMsgContainer*>* fifo,
                    const string& local_ip,
                    const string& local_interface_to_bind_to,
                    int port = SIP_PORT);
        ///
        ~SipUdp_impl();
        ///
        int udpSend(SipMsgContainer* msg);
        ///
        void send(SipMsgContainer* msg, const Data& host="",
		  const Data& port="5060");
        ///
        static void reTransOff();
        ///
        static void reTransOn();
        ///
        static void setRetransTime(int initial = retransmitTimeInitial , int max = retransmitTimeMax );
        ///
        void setRandomLosePercent(int percent);
        ///
        void printSize();
        ///
        Data getDetails() const;

        // Returns the configured IP, if it is not "", otherwise, returns
        // default system IP address.
        const string getLocalIp() const;

        static int getInstanceCount() { return atomic_read(&_cnt); }

    private:
        ///
        static bool Udpretransmitoff;
        ///
        static int  Udpretransmitimeinitial;
        ///
        static int Udpretransmittimemax;
        
        static atomic_t _cnt;

        ///
        int randomLosePercent;
        ///
        UdpStack udpStack;
        ///
        Fifo<SipMsgContainer * >* recFifo;
        ///
        Fifo<RetransmitContents *> sendFifo;
        /// 
        VThread sendThread;
        ///
        VThread receiveThread;
        ///
        bool shutdown;                                       
        ///
        static void* sendThreadWrapper(void *p);
        /// 
        static void* rcvThreadWrapper(void *p);
        ///    
        void* receiveMain();
        ///    
        void* sendMain();
        ///        
        void getHostPort(Sptr<SipMsg> sipMessage, Data* host, int* port);
        ///

        SipUdp_impl();
        SipUdp_impl(const SipUdp_impl& src);
        SipUdp_impl& operator =(const SipUdp_impl& src) const;
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
