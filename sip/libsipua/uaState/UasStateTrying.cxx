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



#include "UasStateTrying.hxx"
#include "UaStateFactory.hxx"
#include "BasicAgent.hxx"
#include "StatusMsg.hxx"
#include "SipCommand.hxx"

using namespace Vocal::UA;
using Vocal::UA::UasStateTrying;

using namespace Vocal;

void 
UasStateTrying::recvRequest(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    cpLog(LOG_DEBUG, "UasStateTrying::recvRequest");
    Sptr<BasicAgent> ba = agent.getControllerAgent();
    if((msg->getType() == SIP_CANCEL) || (msg->getType() == SIP_BYE)) {
       //Send 200 
       //Sptr<StatusMsg> status =
       agent.sendReplyForRequest(msg, 200);
       //agent.setResponse(status); // VEER
       if (msg->getType() == SIP_CANCEL) {
          agent.sendReplyForRequest(agent.getRequest(), 487);
          if (ba != 0) {
             ba->doCancel();
          }
       }
       //Notify CC
       if (ba != 0) {
          ba->receivedRequest(agent, msg, "UasStateTrying");
       }
       changeState(agent, UaStateFactory::instance().getState(U_STATE_FAILURE));
    }
    else if (msg->getType() == SIP_INVITE) {
       changeState(agent, UaStateFactory::instance().getState(U_STATE_S_TRYING));
    }
}

int
UasStateTrying::sendStatus(UaBase& agent, Sptr<SipMsg> msg, const char* dbg)
                 throw (CInvalidStateException&)
{
    Sptr<StatusMsg> statusMsg;
    statusMsg.dynamicCast(msg);
    assert(statusMsg != 0);
    int statusCode = statusMsg->getStatusLine().getStatusCode();
    //Only handle status code 18x, 2xx and above
    if (statusCode == 100) {
        //Do nothing
        return -1;
    }


    if ((statusCode >= 180) && (statusCode < 200)) {
        //Send status message
        Sptr<SipSdp> sipSdp;
        sipSdp.dynamicCast(msg->getContentData(0));
        agent.sendReplyForRequest(agent.getRequest(), statusCode, sipSdp.getPtr());
        //Transit to ringing
        changeState(agent, UaStateFactory::instance().getState(U_STATE_RINGING));
    }
    // 14/1/04 fpi
    // BugFix Bugzilla 780
    // contact field must be sent to allow the ua
    // placing the call to redirected number
    else if ((statusCode >= 300) && (statusCode < 400)) {
       Sptr<SipCommand> sipCmd;
       sipCmd.dynamicCast(agent.getRequest());
       assert(sipCmd != 0);
       Sptr<StatusMsg> sendSMsg = new StatusMsg(*sipCmd, statusCode);
		 
       SipContact me = msg->getContact();
       sendSMsg->setNumContact( 0 );
       sendSMsg->setContact(me);
		 
       cpLog(LOG_DEBUG, "(%s) sending status %s", className().c_str(), sendSMsg->encode().logData());
       agent.getSipTransceiver()->sendReply(sendSMsg, dbg);
       agent.setResponse(sendSMsg.getPtr());

       changeState(agent, UaStateFactory::instance().getState(U_STATE_FAILURE));
    }
    else if (statusCode >= 400)
    {
       //Send status message
       agent.sendReplyForRequest(agent.getRequest(), statusCode);
       //Transit to Failure
       changeState(agent, UaStateFactory::instance().getState(U_STATE_FAILURE));
    }
    else if(statusCode == 200)
    {
        //Send status message
        ///Manipulate SDP data
        Sptr<SipSdp> sipSdp;
        sipSdp.dynamicCast(msg->getContentData(0));

        Sptr<SipCommand> sipCmd;
        sipCmd.dynamicCast(agent.getRequest());
        assert(sipCmd != 0);
        Sptr<StatusMsg> sendSMsg = new StatusMsg(*sipCmd, statusCode);

        //Data receivedIp = msg->getReceivedIPName();
        //agent.fixSdpForNat(msg, receivedIp);

        sendSMsg->setNumContentData(0);
        if(sipSdp != 0) {
            sendSMsg->setContentData(sipSdp.getPtr());
            agent.setLocalSdp(sipSdp);
        }
        //// Find a nice way to do it
        Sptr<BaseUrl> bUrl = sipCmd->getTo().getUrl();
        Sptr<SipUrl> mUrl;
        mUrl.dynamicCast(bUrl);
        assert(mUrl != 0);

        mUrl->setHost(agent.getNatHost());
        mUrl->setPort(Data(agent.getMySipPort()));

        SipContact me("", agent.getMyLocalIp());
        me.setUrl(mUrl.getPtr());
        sendSMsg->setNumContact( 0 );
        sendSMsg->setContact( me );

        cpLog(LOG_DEBUG, "(%s) sending status %s", className().c_str(), sendSMsg->encode().logData());
        agent.getSipTransceiver()->sendReply(sendSMsg, dbg);
	agent.setResponse(sendSMsg.getPtr());
        Sptr<BasicAgent> ba = agent.getControllerAgent();
        if (ba != 0) {
           ba->inCall();
        }
        agent.setCallLegState(C_LIVE);
        //Transit to ringing
        changeState(agent, UaStateFactory::instance().getState(U_STATE_INCALL));
    }
    return 0;
}


