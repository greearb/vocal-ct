#ifndef SipTransceiver_HXX
#define SipTransceiver_HXX

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



static const char* const SipTransceiver_hxx_Version
= "$Id: SipTransceiver.hxx,v 1.8 2004/09/30 23:17:43 greear Exp $";


#include <string>
#include "global.h"
#include "TransceiverSymbols.hxx"

#include "SipTcpConnection.hxx"
#include "SipUdpConnection.hxx"

#include "SipTransactionDB.hxx"
#include "SipSentRequestDB.hxx"
#include "SipSentResponseDB.hxx"
#include "SipTransactionId.hxx"

#include "SipSnmpDetails.hxx"
#include "SipMsgQueue.hxx"
#include "SipMsg.hxx"
#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "Sptr.hxx"

namespace Vocal
{
    
///
typedef enum
{
   APP_CONTEXT_GENERIC,
   APP_CONTEXT_PROXY
}SipAppContext;


/**
 * SipTransceiver is the main class for users the SIP stack.
 *  It is the object
 *  which the caller uses to send and receive SIP messages.
 */
class SipTransceiver: public BugCatcher {
public:
   /**
    * hashTableSizeHint is what we'll use for the initial number of hash table
    * buckets.
    * @param local_dev_to_bind_to  If not "", we'll bind to this
    *  device with SO_BINDTODEV
    */
   SipTransceiver(const string& local_ip,
                  const string& local_ip_to_bind_to,
                  Data s, /* = 0 */
                  int sipPort,/* = SIP_PORT, */
                  bool nat/* = false*/ , 
                  SipAppContext aContext/*=APP_CONTEXT_GENERIC*/,
                  bool blocking);

   ///
   virtual ~SipTransceiver();
   
   /**
    * Copy on write is "expensive", can we change this interface,
    * specially since we know that most of these are in Sptr's
    * already
    */
   virtual void sendAsync(Sptr<SipCommand> sipMessage);

   ///Interface so that we do not have to copy message again in stack
   void sendAsync(Sptr<SipCommand> sipMessage, const Data& host/*=""*/,
                  const Data& port /*=""*/);
   
   ///Interface so that we do not have to copy message again in stack
   virtual void sendReply(Sptr<StatusMsg> sipMessage);

   /**
    ** Return a queue of SipMsgs, basically containing the msg chain.
    *  Will not block (will return NULL if cannot read anything)
    */
   virtual Sptr < SipMsgQueue > receiveNB();
   
   ///
   static void reTransOff();

   ///
   static void reTransOn();

	///
   static void setRetransTime(int initial = retransmitTimeInitial ,
                              int max = retransmitTimeMax 
                              /* Default values in TranscevierSymbols.hxx*/ );

   /// Used for debugging
   void setRandomLosePercent(int percent);

   const string getLocalIp() const { return localIp; }

   ///
   void updateSnmpData(Sptr < SipMsg > sipMsg, SnmpType snmpType);

   ///
   Sptr<SipCallContainer> getCallContainer(const SipTransactionId& id);

   ///
   void printSize();
   
   ///
   static SipAppContext myAppContext;

   ///
   Data getLocalNamePort() const;

   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);

   static int getInstanceCount() { return atomic_read(&_cnt); }
   
private:
   ///
   SipTransceiver();
   SipTransceiver(const SipTransceiver&);
   ///
   SipTransceiver& operator =(const SipTransceiver& src);
  
   Sptr < SipUdpConnection > udpConnection;

   Sptr < SipTcpConnection > tcpConnection;

   SipSentRequestDB sentRequestDB;

   SipSentResponseDB sentResponseDB;

   ///
   void send(Sptr<SipMsgContainer> msgPtr, const Data& host,
             const Data& port);

   void send(Sptr<SipMsgContainer> msgPtr);
  
   /// SNMP details data member vriables
   Sptr<SipAgent> sipAgent;

   Data myLocalNamePort;
   
   Data application;
  
   bool natOn;
   
   string localIp;

   static atomic_t _cnt;
};

} // namespace Vocal

#endif
