#ifndef _Sip_CALL_CONTAINER_HXX
#define _Sip_CALL_CONTAINER_HXX

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
#include "SipMsgContainer.hxx"
#include "SipMsg.hxx"
#include "NetworkAddress.h"
#include "cpLog.h"
#include <misc.hxx>
#include <list>

namespace Vocal
{
    
#define DEBUG_NEW_STACK LOG_DEBUG_STACK


class SipCallContainer : public BugCatcher {
public:

   SipCallContainer(const SipTransactionId& call_id);

   virtual ~SipCallContainer() { _cnt--; }

   void addMsgPair(Sptr<SipMsgPair> m);

   Sptr<SipMsgPair> findMsgPair(const SipTransactionId& id);
   Sptr<SipMsgPair> findMsgPair(Method method);

   void stopAllRetrans();

   void clear(const char* debug);

   list<Sptr<SipMsgPair> >& getMsgList() { return msgs; }

   int getCurSeqNum() { return curSeqNum; }
   void setCurSeqNum(int i) { curSeqNum = i; setSeq = true; }

   bool isSeqSet() { return setSeq; }

   const SipTransactionId& getTransactionId() { return id; }

   static int getInstanceCount() { return _cnt; }

   void setPurgeTimer(uint64 t) { purgeAt = t; }
   uint64 getPurgeTimer() const { return purgeAt; }

protected:
   list<Sptr<SipMsgPair> > msgs;
   SipTransactionId id;

   int curSeqNum; //Sequence number of last SIP message received.
   bool setSeq; // Has it been set yet?
   uint64 purgeAt; // Purge call if this time is non zero and is in the past

private:
   // Not implemented
   SipCallContainer();
   SipCallContainer(const SipCallContainer& rhs);
   SipCallContainer& operator=(const SipCallContainer& rhs);

   static unsigned int _cnt;
};//SipCallContainer
 
} // namespace Vocal

#endif

