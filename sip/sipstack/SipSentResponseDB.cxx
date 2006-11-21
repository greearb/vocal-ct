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

#include "global.h"
#include "SipTransceiver.hxx"
#include "SipSentResponseDB.hxx"
#include "SipMsg.hxx"
#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "SipVia.hxx"

using namespace Vocal;

SipSentResponseDB::SipSentResponseDB(const string& _local_ip)
      : SipTransactionDB(_local_ip, "sent-response-db")
{}

SipSentResponseDB::~SipSentResponseDB()
{
    /// the SipTransactionDB::~ will do the clean up
}

Sptr<SipMsgContainer>
SipSentResponseDB::processSend(const Sptr<SipMsg>& msg) {
   /// the only send in THIS db can be of the responses
   Sptr<StatusMsg> response;
   response.dynamicCast(msg);
   assert(response != 0);

   SipTransactionId id(*msg);

   Sptr<SipMsgContainer> retVal = new SipMsgContainer(id);
   retVal->setMsgIn(msg);
   retVal->setTransport(msg->getVia(0).getTransport().c_str());

   // if its a final response then update the transactionDB
   if (response->getStatusLine().getStatusCode() >= 200) {

      // Find the related call
      Sptr<SipCallContainer> call = getCallContainer(id);
      if (call != 0) {
         Sptr<SipMsgPair> mp = call->findMsgPair(id);
         if (mp != 0) {
            if (mp->response != 0) {
               cpLog(LOG_ERR, "Collision\nOLD:%s\n\nNEW:%s",
                     mp->response->toString().c_str(),
                     response->toString().c_str());
            }
            else {
               mp->response = retVal;
               // make the transport retrans it repeatedly
               if ((response->getType() == SIP_INVITE) &&
                   (SipTransceiver::myAppContext != APP_CONTEXT_PROXY)) {
                  cpLog( LOG_DEBUG_STACK, "Set UA INVITE final response retransmission" );
                  retVal->setRetransmitMax(MAX_RETRANS_COUNT);
               }
            }
         }
      }
   }

   return retVal;
}


Sptr<SipMsgQueue>
SipSentResponseDB::processRecv(Sptr<SipMsgContainer> msgContainer) {
   // the only receive in THIS db can be of the requests
   assert(dynamic_cast<SipCommand*>(msgContainer->getMsgIn().getPtr()));

   SipTransactionId id(*(msgContainer->getMsgIn()));
   Sptr<SipMsgQueue> retVal;
    
   Sptr<SipCallContainer> call = getCallContainer(id);
   Sptr<SipMsgPair> mp;

   if (call == 0) {
      // See if we can create a new call.
      if (msgContainer->getMsgIn()->getType() == SIP_INVITE) {
         call = new SipCallContainer(id);
         addCallContainer(call);
      }
   }

   if (call == 0) {
      // there was no transaction found/created for this message,
      // so just proxy it up and hope for the best!!!
      // TODO:  This looks a bit strange.  If we should proxy, we should KNOW
      //  we needed to proxy..otherwise, should drop. --Ben
      retVal = new SipMsgQueue();
      retVal->push_back(msgContainer->getMsgIn());
      cpLog(LOG_ERR, "No trasaction for %s",
            msgContainer->getMsgIn()->toString().c_str());
   }
   else {

      mp = call->findMsgPair(id);
      if (mp == 0) {
         mp = new SipMsgPair();
      }

      if (mp->request != 0) {
         // if there is a coresponding response then retrans it
         if (mp->response != 0) {
            cpLog(LOG_INFO,"duplicate message: %s",
                  msgContainer->toString().c_str());

            // Just assign the new message in place.
            mp->response = msgContainer;
            msgContainer->setRetransmitMax(FILTER_RETRANS_COUNT);
         }
         else {
            /// we didn't find a matching response but since the request
            /// has been received already so assume that application will
            /// be sending a response (if it's not an ACK)
            /// and hence just drop this duplicate
            msgContainer->clear();
         }
      }
      else {
         cpLog(LOG_DEBUG_STACK, "First time receive of this message...");
         // this is a first time receive of this message
         int msgSeqNumber 
            = msgContainer->getMsgIn()->getCSeq().getCSeqData().convertInt();
         int storedSeqNum = call->getCurSeqNum();
           
         if (SipTransceiver::myAppContext != APP_CONTEXT_PROXY) {
            if (!((msgContainer->getMsgIn()->getType() == SIP_ACK) ||
                  (msgContainer->getMsgIn()->getType() == SIP_CANCEL) )) {
                 
               // Not ACK or CANCEL
               if (call->isSeqSet()) {
                  // Check the seq number of the message for the same call-leg.
                    
                  // 1.  if the seq number is higher than previous one Msg is OK
                  // 2.  If sequence number is lower, it is an error
                  // 3.  If sequence number is equal and message is not an
                  //     ACK or CANCEL it is an error.  ACK and CANCELs w/
                  //     the same seqence number are OK.

                  if (msgSeqNumber > storedSeqNum) {
                     cpLog(LOG_DEBUG, "***** Bumping seq to %d", msgSeqNumber);
                     //Bump up the next sequenece only if not ACK or CANCEL      
                     call->setCurSeqNum(msgSeqNumber);
                  }
                  else if( (msgSeqNumber < storedSeqNum) ||
                           (msgSeqNumber == storedSeqNum) ) {             
                     // Error condition
                     Sptr<SipCommand> sipCmd;
                     sipCmd.dynamicCast(msgContainer->getMsgIn());
                     Sptr<StatusMsg> statusMsg = new StatusMsg(*sipCmd, 400); 
                     Data reason = "Sequence number out of order";
                     statusMsg->setReasonPhrase(reason);
                     Sptr<SipMsg> sm;
                     sm.dynamicCast(statusMsg);
                     msgContainer->setMsgIn(sm);
                     msgContainer->setTransport(statusMsg->getVia(0).getTransport().c_str());

                     msgContainer->setRetransmitMax(1);
                     
                     processSend(statusMsg.getPtr());
                     return 0;
                  }
               }
            }
            else {
               // I see no reason to set this to older values.. --Ben
               if (msgSeqNumber > storedSeqNum) {                    
                  call->setCurSeqNum(msgSeqNumber);
                  cpLog(LOG_DEBUG_STACK, "***** Setting seq to %d for callId %s",
                        msgSeqNumber, id.toString().c_str());
               }
            }
         }//if not a proxy

         // construct the msg queue to be sent up to application
         retVal = new SipMsgQueue();
         if (msgContainer->getMsgIn()->getType() == SIP_ACK) {

            // TODO:  It seems that this only (fully) handles response to
            //  an invite?

            mp = call->findMsgPair(SIP_INVITE);

            if (mp != 0) {
               cpLog(LOG_DEBUG_STACK,"Found INVITE");
               // add the other items of this transaction, and
               // reduce memory usage by clearing the parsed
               // message in the in pointer

               if (mp->request != 0) {
                  retVal->push_back(mp->request->getMsgIn());
               }
               if (mp->response != 0) {
                  retVal->push_back(mp->response->getMsgIn());
                  mp->response->setRetransmitMax(0); // Cancel retrans of response
                  // also cancel the retrans of response
                  cpLog(DEBUG_NEW_STACK,"Stopping retrans of response[%s]",
                        mp->response->toString().c_str());
               }
            }
            else {
               cpLog(LOG_DEBUG_STACK, "INVITE not Found");
               // ACK w/o INVITE !!!!
               // (may have been gc'd)
            }
         }

         retVal->push_back(msgContainer->getMsgIn());
      }
   }
   return retVal;
}

