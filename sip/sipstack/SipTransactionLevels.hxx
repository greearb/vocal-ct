#ifndef _Sip_Transaction_Levels__hxx
#define _Sip_Transaction_Levels__hxx

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

static const char* const SipTransactionLevels_hxx_version =
    "$Id: SipTransactionLevels.hxx,v 1.5 2004/06/01 07:23:31 greear Exp $";

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
    /// the request
    Sptr<SipMsgContainer> request;
    /// and the response
    Sptr<SipMsgContainer> response;
};


class SipMsgContainer : public BugCatcher {
public:
   // constructed with default value for stateless mode
   SipMsgContainer(const SipTransactionId& id);

   void cleanup();

   string toString() const;

   void setRetransCount(int i) { retransCount = i; }
   int getRetransCount() { return retransCount; }

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

protected:
   // refence to the message (for incoming messages)
   Sptr<SipMsg> in;

   // cache of raw msg txt (for outgoing messages)
   string in_encode;

   //Host
   Sptr<NetworkAddress> netAddr;

   //
   string  transport;

   uint64 shouldGcAt; //When should we garbage-collect this thing.  Used by SipTransactionGC
   int retransCount;
   bool collected;
   int prepareCount; /** How many times have we tried to send this
                      * message.  Used by SipTcpStack at least.
                      */
   SipTransactionId trans_id;

}; //class SipMsgContainer


class SipCallContainer : public BugCatcher {
public:

   SipCallContainer(const SipTransactionId& call_id);

   virtual ~SipCallContainer() { }

   void addMsgPair(Sptr<SipMsgPair> m) {
      msgs.push_back(m);
   }

   Sptr<SipMsgPair> peekTopMsg() {
      if (msgs.size()) {
         return msgs.front();
      }
      return NULL;
   }

   Sptr<SipMsgPair> findMsgPair(const SipTransactionId& id);
   Sptr<SipMsgPair> findMsgPair(Method method);

   void popMsg() {
      assert(msgs.size());
      msgs.pop_front();
   }
   
   void clear();

   list<Sptr<SipMsgPair> >& getMsgList() { return msgs; }

   int getCurSeqNum() { return curSeqNum; }
   void setCurSeqNum(int i) { curSeqNum = i; setSeq = true; }

   bool isSeqSet() { return setSeq; }

   const SipTransactionId& getTransactionId() { return id; }

protected:
   list<Sptr<SipMsgPair> > msgs;
   SipTransactionId id;

   int curSeqNum; //Sequence number of last SIP message received.
   bool setSeq; // Has it been set yet?

private:
   // Not implemented
   SipCallContainer();
   SipCallContainer(const SipCallContainer& rhs);
   SipCallContainer& operator=(const SipCallContainer& rhs);

};//SipCallContainer
 
} // namespace Vocal

#endif
