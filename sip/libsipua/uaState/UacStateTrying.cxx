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


#include "UacStateTrying.hxx"
#include "UaStateFactory.hxx"
#include "BasicAgent.hxx"
#include "AckMsg.hxx"
#include "CancelMsg.hxx"

using namespace Vocal::UA;
using Vocal::UA::UacStateTrying;

int UacStateTrying::sendRequest(UaBase& agent, Sptr<SipMsg> msg, const char* dbg) {
   cpLog(LOG_DEBUG, "UacStateTrying::sendRequest");
   if ((msg->getType() == SIP_CANCEL) ||
      (msg->getType() == SIP_BYE)) {
      Sptr<SipCommand> sipCmd;
      sipCmd.dynamicCast(agent.getRequest());
      Sptr<CancelMsg> cMsg = new CancelMsg(*sipCmd); 
      cpLog(LOG_DEBUG, "Sending cancel:%s" , cMsg->encode().logData());
      agent.getSipTransceiver()->sendAsync(cMsg.getPtr(), dbg);
      changeState(agent, UaStateFactory::instance().getState(U_STATE_FAILURE));
      return 0;
   }
   else if (msg->getType() == SIP_INVITE) {
      Sptr<SipCommand> sipCmd;
      sipCmd.dynamicCast(msg);
      if (sipCmd.getPtr()) {
         // Valid reason includes having to authenticate the previous INVITE that got us into this state...
         cpLog(LOG_ERR, "WARNING:  Sending SIP_INVITE from UacStateTrying, assume is Authentication.\n");
         agent.getSipTransceiver()->sendAsync(sipCmd.getPtr(), dbg);
         cpLog(LOG_ERR, "WARNING:  Done sending SIP_INVITE from UacStateTrying.\n");
         return 0;
      }
      else {
         cpLog(LOG_ERR, "Couldn't convert msg:\n%s\nto a SipCommand!\n", msg->toString().c_str());
         return -EINVAL;
      }
   }
   else {
      cpLog(LOG_ERR, "Expecting Cancel or Bye, got wrong message type: %d  msg:\n%s\n",
            msg->getType(), msg->toString().c_str());
      return -EINVAL;
   }
}

void 
UacStateTrying::recvStatus(UaBase& agent, Sptr<SipMsg> msg) {
   Sptr<StatusMsg> statusMsg;
   statusMsg.dynamicCast(msg);
   assert(statusMsg != 0);
   int statusCode = statusMsg->getStatusLine().getStatusCode();
   cpLog(LOG_DEBUG, "UacStateTrying::recvStatus:%d", statusCode);
   if (statusCode == 200 ) {
      //Save final response for future BYE
      agent.setResponse(msg);
      agent.saveRouteList(msg, true);
      Sptr<SipSdp> sdp;
      sdp.dynamicCast(msg->getContentData(0));
      agent.setRemoteSdp(sdp);
      
      //Received a request, get the received IP and
      //Set it in SDP
      Data receivedIp = msg->getReceivedIPName();
      // Agent may choose not to make changes.
      agent.fixSdpForNat(msg, receivedIp);

      //Notify CC
      Sptr<BasicAgent> ba = agent.getControllerAgent();
      if (ba != 0) {
         ba->receivedStatus(agent, msg, "UacStateTrying");
      }
      agent.setCallLegState(C_LIVE);
      //Transit to Incall
      changeState(agent, UaStateFactory::instance().getState(U_STATE_INCALL));
   }
   else if (statusCode > 200) {
      if (statusCode != 408) {
         //Send ACK message
         Sptr<AckMsg> ackMsg = new AckMsg(*statusMsg, agent.getMyLocalIp());
         SipRequestLine& ackRequestLine = ackMsg->getMutableRequestLine();
         Sptr<SipCommand> sCommand;
         sCommand.dynamicCast(agent.getRequest());
         assert(sCommand != 0);
         addSelfInVia(agent, ackMsg.getPtr());
         ackRequestLine.setUrl(sCommand->getRequestLine().getUrl());
         agent.getSipTransceiver()->sendAsync(ackMsg.getPtr(), "UAC Trying:  408");
         cpLog(LOG_INFO, "Sent Ack for status (%d), ack-msg:\n%s\n" ,
               statusCode, ackMsg->encode().logData());
      }

      // Deal with AUTH requests.
      if ((statusCode == 401) || (statusCode == 407)) {
         // Don't change state..basic agent will send the auth.
         cpLog(LOG_ERR, "NOTE:  Received auth-needed status code: %d.  Not changing state, assume agent will authenticate.\n", statusCode);
      }
      else if (statusCode == 302){
         SipContactList contactList = statusMsg->getContactList();
         if (!contactList.empty()) {
            changeState(agent, UaStateFactory::instance().getState(U_STATE_REDIRECT)); 
         }
         else {
            changeState(agent, UaStateFactory::instance().getState(U_STATE_IDLE)); 
            Sptr<BasicAgent> ba = agent.getControllerAgent();
            if (ba != 0) {
               ba->callFailed();
            }
            return;
         }
      }
      else{
         //Transit to Idle
         changeState(agent, UaStateFactory::instance().getState(U_STATE_IDLE));
      }

      //Notify CC
      Sptr<BasicAgent> ba = agent.getControllerAgent();
      if (ba != 0) {
         ba->receivedStatus(agent, msg, "UacStateTrying > 200");
      }
   }
   else {
      //For 1xx
      //Notify CC
      if ((statusCode >= 180)) {
         //Transit to ringing
         changeState(agent, UaStateFactory::instance().getState(U_STATE_RINGING));
      }
      Sptr<BasicAgent> ba = agent.getControllerAgent();
      if (ba != 0) {
         ba->receivedStatus(agent, msg, "UacStateTrying 1xx");
      }
   }
}


void 
UacStateTrying::recvRequest(UaBase& agent, Sptr<SipMsg> msg)
{
    cpLog(LOG_DEBUG, "(%s) receiving message (%s) give it to controller",
          agent.getInstanceName().c_str(), msg->encode().logData());
    Sptr<BasicAgent> ba = agent.getControllerAgent();
    if (ba != 0) {
       ba->receivedRequest(agent, msg, "UacStateTrying");
    }
}
