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



static const char* const UaStateFailure_cxx_Version =
    "$Id: UaStateFailure.cxx,v 1.3 2004/10/29 07:22:35 greear Exp $";

#include "UaStateFailure.hxx"
#include "UaBase.hxx"
#include "BasicAgent.hxx"
#include "UaStateFactory.hxx"
#include "AckMsg.hxx"
#include "SipVia.hxx"

using namespace Vocal;
using namespace UA;

using namespace Vocal;

void 
UaStateFailure::recvRequest(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
   cpLog(LOG_DEBUG, "UaStateFailure::recvRequest");
   if (msg->getType() == SIP_ACK) {
      if (agent.isAServer()) {
         // 13/1/04 fpi
         // BugFix BugZilla 773
         Sptr<BasicAgent> ba = agent.getControllerAgent();
         if (ba != 0) {
            ba->callFailed();
         }
         //Transit to Idle
         changeState(agent, UaStateFactory::instance().getState(U_STATE_IDLE));
      }
      else {
         cpLog(LOG_ERR, "Unexpected message type %d, while expecting ACK", msg->getType());
      }
   }
}


void 
UaStateFailure::recvStatus(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    cpLog(LOG_DEBUG, "UaStateFailure::recvStatus");
    assert(agent.isAClient());
    Sptr<StatusMsg> statusMsg;
    statusMsg.dynamicCast(msg);
    assert(statusMsg != 0);
    int statusCode = statusMsg->getStatusLine().getStatusCode();
    if(statusCode > 200)
    {
        //Transit to Idle
        if(statusCode == 487)
        {
            //Create the ACK msg of the original Invite and send
            Sptr<InviteMsg> invMsg;
            invMsg.dynamicCast(agent.getRequest());
            assert(invMsg != 0);
            Sptr<AckMsg> ackMsg = new AckMsg(*statusMsg, agent.getMyLocalIp());
            ackMsg->flushViaList();

            SipVia via = invMsg->getVia(0);
            ackMsg->setVia(via, 0);

            Sptr<SipUrl> sUrl;
            sUrl.dynamicCast(invMsg->getRequestLine().getUrl());
            SipRequestLine rLine(SIP_ACK, sUrl.getPtr(), agent.getMyLocalIp());
            ackMsg->setRequestLine(rLine);

            agent.getSipTransceiver()->sendAsync(ackMsg.getPtr());
        }
        //Notify CC
        Sptr<BasicAgent> ba = agent.getControllerAgent();
        if (ba != 0) {
           ba->receivedStatus(agent, msg);
           ba->callFailed();
        }
        changeState(agent, UaStateFactory::instance().getState(U_STATE_IDLE));
    }
}

int
UaStateFailure::sendStatus(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    //DO nothing
   return -1;
}

int UaStateFailure::sendRequest(UaBase& agent, Sptr<SipMsg> msg) 
   throw (CInvalidStateException&) {
   cpLog(LOG_ERR, "ERROR:  called sendRequest in UaStateFailure, msg: %s",
         msg->toString().c_str());
   return -1;
}
