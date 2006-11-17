#ifndef _Sip_MSG_CONTAINER_HXX
#define _Sip_MSG_CONTAINER_HXX

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


#include "SipTransactionId.hxx"
#include "SipMsg.hxx"
#include "NetworkAddress.h"
#include "cpLog.h"
#include <misc.hxx>
#include <list>

namespace Vocal
{
    
#define DEBUG_NEW_STACK LOG_DEBUG_STACK

class SipMsgContainer;

static const char * const NOVAL = "";


class SipMsgPair : public BugCatcher {
public:
   SipMsgPair() { _cnt++; }
   virtual ~SipMsgPair() { _cnt--; }

   /// the request
   Sptr<SipMsgContainer> request;
   /// and the response
   Sptr<SipMsgContainer> response;

   static int getInstanceCount() { return _cnt; }

private:
   static unsigned int _cnt;
};




class SipMsgContainer : public BugCatcher {
public:
   // constructed with default value for stateless mode
   SipMsgContainer(const SipTransactionId& id);
   virtual ~SipMsgContainer() { _cnt--; }

   void cleanup();

   string toString() const;

   uint64 getGcAt() { return shouldGcAt; }
   void setGcAt(uint64 v) { shouldGcAt = v; }

   bool hasBeenCollected() { return collected; }
   void setCollected(bool v) { collected = v; }

   void setMsgIn(Sptr<SipMsg> inm);
   Sptr<SipMsg> getMsgIn() { return in; }

   void setTransport(const string& t) { transport = t; }
   const string& getTransport() { return transport; }

   void setNetworkAddr(Sptr<NetworkAddress> na) { netAddr = na; }
   Sptr<NetworkAddress> getNetworkAddr() { return netAddr; }

   virtual void clear();

   virtual bool matches(const SipTransactionId& id);

   // Ensure that the msg encode is current.
   void cacheEncode() {
      in_encode = in->encode().c_str();
   }

   const string& getEncodedMsg() { return in_encode; }

   // From the RetransmitContents class originally.
   void setRetransmitMax(int i) { retransCount = i; }
   int getRetransmitMax() const { return retransCount; }
   void setRetransSoFar(int i) { retransSoFar = i; }
   int getRetransSoFar() { return retransSoFar; }
   int incRetransSoFar() { retransSoFar++; return retransSoFar; }

   const uint64& getNextTx() const { return nextTx; }
   void setNextTx(uint64 next_tx) { nextTx = next_tx; }

   int getLastWaitPeriod() { return wait_period; }
   void setWaitPeriod(int t) { wait_period = t; }

   static int getInstanceCount() { return _cnt; }

protected:
   // refence to the message (for incoming messages)
   Sptr<SipMsg> in;

   // cache of raw msg txt (for outgoing messages)
   string in_encode;

   //Host
   Sptr<NetworkAddress> netAddr;
   string  transport;

   uint64 shouldGcAt; //When should we garbage-collect this thing.  Used by SipTransactionGC
   int retransCount; //maximum
   int retransSoFar; //how many times have we re-transmitted so far?
   bool collected;
   int prepareCount; /** How many times have we tried to send this
                      * message.  Used by SipTcpStack at least.
                      */
   SipTransactionId trans_id;

   // From the RetransmitContents object.
   Sptr<SipMsgContainer> sipMsg;
   uint64 nextTx;
   int wait_period;
private:
   static unsigned int _cnt;

   SipMsgContainer();
   SipMsgContainer(const SipMsgContainer& rhs);
}; //class SipMsgContainer


// We want the lower time to be first in the heap, used to sort
// the transmit heap.
struct SipMsgContainerComparitor {
   bool operator()(Sptr<SipMsgContainer> a, Sptr<SipMsgContainer> b) {
      return (b->getNextTx() < a->getNextTx());
   }
};


} // namespace Vocal

#endif

