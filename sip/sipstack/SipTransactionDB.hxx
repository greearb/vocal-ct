#ifndef _Sip_Transaction_DB__hxx
#define _Sip_Transaction_DB__hxx

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

static const char* const SipTransactionDB_hxx_version =
    "$Id: SipTransactionDB.hxx,v 1.7 2004/11/08 20:39:13 greear Exp $";

#include "SipTransactionId.hxx"
#include "SipMsgContainer.hxx"
#include "SipMsgQueue.hxx"

namespace Vocal
{
    
#define FILTER_RETRANS_COUNT 1
#define MAX_RETRANS_COUNT    7

class SipTransactionDB : public BugCatcher {
private:
   SipTransactionDB();
   SipTransactionDB(const SipTransactionDB&);
   SipTransactionDB& operator= (const SipTransactionDB&);
   bool operator==(const SipTransactionDB&) const;

public:
   // local_ip cannot be "" here, must be the local IP we are bound to locally
   // or 'hostaddress' if we are not specifically bound.
   SipTransactionDB(const string& _local_ip);
   virtual ~SipTransactionDB();

   /**
    * entry point for transceiver functionality
    * it will be passed the Sptr reference from worker thread, and will return
    * a pointer to a newly created Sip message container that needs to be passed
    * to the transport layer
    */
   virtual Sptr<SipMsgContainer> processSend(const Sptr<SipMsg>& msg) = 0;

   /**
    *  entry point for filter functionality
    * it will be given the Sip message container received from transport layer,
    * and either will return the transaction queue, or will return null and modify
    * the Sip message container w/ the old message to be retransmitted
    *
    * (if the message is droped then msg queue AND the reference to msg in the
    * container will both be 0) 
    */
   virtual Sptr<SipMsgQueue> processRecv(Sptr<SipMsgContainer> msgContainer) = 0;

   string toString();

   Sptr<SipCallContainer> getCallContainer(const SipTransactionId& id);

   void addCallContainer(Sptr<SipCallContainer> cc);

   void purgeOldCalls(uint64 now);

   void setPurgeTimer(const SipTransactionId& id);

protected:

   map <SipTransactionId::KeyTypeII, Sptr<SipCallContainer> > table;

   string local_ip;
};
 
} // namespace Vocal

#endif
