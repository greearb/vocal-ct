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
    "$Id: SipTransactionDB.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipTransactionId.hxx"
#include "SipTransHashTable.hxx"
#include "SipTransactionList.hxx"
#include "SipTransactionLevels.hxx"
#include "SipTransactionGC.hxx"
#include "SipMsgQueue.hxx"


namespace Vocal
{
    
#define FILTER_RETRANS_COUNT 1
#define MAX_RETRANS_COUNT    7

class SipTransactionDB
{
public:
    // local_ip cannot be "" here, must be the local IP we are bound to locally
    // or 'hostaddress' if we are not specifically bound.
    // Size is the size of the underlying hash-table (buckets).
    SipTransactionDB(int size, const string& _local_ip);
    virtual ~SipTransactionDB();

private:
    SipTransactionDB();
    SipTransactionDB(const SipTransactionDB&);
    SipTransactionDB& operator= (const SipTransactionDB&);
    bool operator==(const SipTransactionDB&) const;

public:
    /// entry point for transceiver functionality
/**
 * it will be passed the Sptr reference from worker thread, and will return
 * a pointer to a newly created Sip message container that needs to be passed
 * to the transport layer
 */
    virtual SipMsgContainer* processSend(const Sptr<SipMsg>& msg) = 0;

    /// entry point for filter functionality
/**
 * it will be given the Sip message container received from transport layer,
 * and either will return the transaction queue, or will return null and modify
 * the Sip message container w/ the old message to be retransmitted
 *
 * (if the message is droped then msg queue AND the reference to msg in the
 * container will both be 0) 
 */
    virtual SipMsgQueue* processRecv(SipMsgContainer* msgContainer) = 0;

  /**
   * this will return all the messages corresponding to the call leg of
   * the sip message in argument
   */
  typedef vector<Sptr <SipMsg> > CallLegVector;
  virtual CallLegVector getCallLegMsgs(Sptr <SipMsg>& sipMsg);

  Data getDetails();

protected:
/**
 * this is NOT private, 'coz want to lock the Bucket node from find/insert
 * during the whole operation using RWHelper. hence, it all has to be in
 * a single scope
 */
    SipTransHashTable table;

/**
 * cancel the retrans of the given message...
 */
    void cancel(SipMsgContainer* msg);

/**
 * get the top node and update state info specific to DB type
 * (needed here 'coz of getCallLegMsgs)
 */
    virtual SipTransLevel1Node* getTopNode(const SipTransactionId& id,
				   const Sptr<SipMsg>& msg) = 0;

        string local_ip;
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
