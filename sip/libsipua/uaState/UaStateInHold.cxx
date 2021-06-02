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


#include "UaStateInHold.hxx"
#include "UaStateFactory.hxx"
#include "SystemInfo.hxx"
#include "BasicAgent.hxx"
#include "AckMsg.hxx"
#include "SystemInfo.hxx"
#include "CancelMsg.hxx"
#include "UaBase.hxx"

using namespace Vocal::UA;

using namespace Vocal;

void 
UaStateInHold::recvStatus(UaBase& agent, Sptr<SipMsg> msg)
{
    cpLog(LOG_DEBUG, "UaStateInHold::recvStatus");
    Sptr<StatusMsg> statusMsg;
    statusMsg.dynamicCast(msg);
    assert(statusMsg != 0);
    int statusCode = statusMsg->getStatusLine().getStatusCode();
    if (statusCode >= 200) {
        //Send Ack
        if (statusMsg->getCSeq().getMethod() == INVITE_METHOD) {
            Sptr<AckMsg> ackMsg = new AckMsg(*statusMsg, agent.getMyLocalIp());
	    addSelfInVia(agent, ackMsg.getPtr());
            agent.getSipTransceiver()->sendAsync(ackMsg.getPtr(), "Uas In Hold, recv Status >= 200");
        }
    }
    if (statusCode == 200) {
	if (statusMsg->getCSeq().getMethod() == INVITE_METHOD) {
	    //Sync up the sequence number
	    agent.getResponse()->setCSeq(statusMsg->getCSeq());
            Sptr<BasicAgent> ba = agent.getControllerAgent();
            if (ba != 0) {
               ba->dohold(); // DO the Hold here, since 200 OK for the
            }
	    // REINVITE Is recevied.
	    return;
	}
    }
    
    //Notify CC   
    Sptr<BasicAgent> ba = agent.getControllerAgent();
    if (ba != 0) {
       ba->receivedStatus(agent, msg, "UaStateInHold");
    }
}

int
UaStateInHold::sendStatus(UaBase& agent, Sptr<SipMsg> msg, const char* dbg)
{
    cpLog(LOG_DEBUG, "UaStateInHold::sendStatus");
    Sptr<StatusMsg> statusMsg;
    statusMsg.dynamicCast(msg);
    assert(statusMsg != 0);
    int statusCode = statusMsg->getStatusLine().getStatusCode();
    if(statusCode == 200)
    {
        Sptr<SipContentData> sdpData = msg->getContentData(0);
        Sptr<SipCommand> sipCmd;
        sipCmd.dynamicCast(agent.getRequest());
        assert(sipCmd != 0);
        Sptr<StatusMsg> sendSMsg = new StatusMsg(*sipCmd, statusCode);

        if(sdpData != 0) {
            sendSMsg->setNumContentData(0);
            sendSMsg->setContentData(sdpData.getPtr());
        }

        Sptr<BaseUrl> bUrl = sipCmd->getTo().getUrl();
        Sptr<SipUrl> mUrl;
        mUrl.dynamicCast(bUrl);
        assert(mUrl != 0);

        mUrl->setHost(agent.getMyLocalIp());
        mUrl->setPort(Data(agent.getMySipPort()));

        SipContact me("", agent.getMyLocalIp());
        me.setUrl(mUrl.getPtr());
        sendSMsg->setNumContact( 0 );
        sendSMsg->setContact( me );
        agent.getSipTransceiver()->sendReply(sendSMsg, dbg);
    }
    else if(statusCode > 200) {
        agent.sendReplyForRequest(agent.getRequest(), statusCode);
    }
    return 0;
}



void 
UaStateInHold::recvRequest(UaBase& agent, Sptr<SipMsg> msg)
{
    cpLog(LOG_DEBUG, "UaStateInHold::recvRequest");
    Sptr<SipCommand> sipCmd;
    sipCmd.dynamicCast(msg);
    assert(sipCmd != 0); 
    switch(msg->getType())
    {
        case SIP_INFO:
        {
            cpLog(LOG_DEBUG, "UaStateInHold, processing SIP_INFO");
            //Send 200 status msg
            agent.sendReplyForRequest(msg, 200);
            //Notify CC
            Sptr<BasicAgent> ba = agent.getControllerAgent();
            if (ba != 0) {
               ba->receivedRequest(agent, msg, "UaStateInHold, INFO");
            }
        }
        break;
        case SIP_BYE:
        {
            cpLog(LOG_DEBUG, "UaStateInHold, processing SIP_BYE");
            //Send 200 
            agent.sendReplyForRequest(msg, 200);
            //Notify CC of the call end
            Sptr<BasicAgent> ba = agent.getControllerAgent();
            if (ba != 0) {
               ba->endCall();
               ba->receivedRequest(agent, msg, "UaStateInHold, BYE");
            }
            //Transit to Idle
            changeState(agent, UaStateFactory::instance().getState(U_STATE_IDLE));
        }
        break;
        case SIP_ACK:
        {
            cpLog(LOG_DEBUG, "UaStateInHold, processing SIP_ACK");
            //Save the Ack message for future BYE
            agent.setAck(msg);
            //Notify CC
            Sptr<BasicAgent> ba = agent.getControllerAgent();
            if (ba != 0) {
               ba->receivedRequest(agent, msg, "UaStateInHold, ACK");
            }
        }
        break;
        case SIP_INVITE:
        {
            cpLog(LOG_DEBUG, "UaStateInHold, processing SIP_INVITE");
            //Send 100 status msg
				
				// 23/11/04 fpi
				// WorkAround BugZilla 779
				// this is a re-invite, if we set the response
				// we are wrong beacause TO and FROM fields
				// are shifted
            agent.sendReplyForRequest(msg, 100, 0, false);

            agent.setRequest(msg);
            //Notify CC
            Sptr<BasicAgent> ba = agent.getControllerAgent();
            if (ba != 0) {
               ba->reqResume(msg);
            }
	    changeState(agent, UaStateFactory::instance().getState(U_STATE_INCALL));
        }
        break;
        case SIP_CANCEL:
        {
            cpLog(LOG_WARNING, "UaStateInHold, processing SIP_CANCEL, this is incorrect");
            //Send 200 
            agent.sendReplyForRequest(msg, 200);
            agent.sendReplyForRequest(msg, 487);
            //Notify CC of the call end
            Sptr<BasicAgent> ba = agent.getControllerAgent();
            if (ba != 0) {
               //Notify CC
               ba->receivedRequest(agent, msg, "UaStateInHold, CANCEL");
               ba->endCall();
            }
            //Transit to Idle
            changeState(agent, UaStateFactory::instance().getState(U_STATE_FAILURE));
        }
        break;
        default:
            cpLog(LOG_WARNING, "Do not know what to do with message type %d:" , msg->getType());
        break;
    }
}

int
UaStateInHold::sendRequest(UaBase& agent, Sptr<SipMsg> msg, const char* dbg)
{
    int rv = 0;
    cpLog(LOG_DEBUG, "UaStateInHold::sendRequest");
    //Send request
    switch(msg->getType())
    {
        case SIP_BYE:
        {
            agent.sendBye();
            //Transit to End
            changeState(agent, UaStateFactory::instance().getState(U_STATE_END));
        }
        break;
        case SIP_INVITE:
        {
            Sptr<InviteMsg> srcMsg;
            srcMsg.dynamicCast(msg);
       
            Sptr<InviteMsg> invMsg ;
            invMsg = agent.createReInvite(srcMsg);
            invMsg->setCallId(agent.getRequest()->getCallId());
            //Set the To tag from the previous final response
            SipTo to = invMsg->getTo();
            if (to.getUser() == agent.getResponse()->getTo().getUser()) {
               to.setTag(agent.getResponse()->getTo().getTag());
            }
            else {
               to.setTag(agent.getResponse()->getFrom().getTag());
            }
            invMsg->setTo(to);

            SipFrom from = invMsg->getFrom();
            if (from.getUser() == agent.getResponse()->getFrom().getUser()) {
                from.setTag(agent.getResponse()->getFrom().getTag());
            }
            else {
                from.setTag(agent.getResponse()->getTo().getTag());
            }
            invMsg->setFrom(from);

            cpLog(LOG_DEBUG, "(%s) Sending re-invite %s",
                  agent.getInstanceName().c_str(), invMsg->encode().logData());
            agent.getSipTransceiver()->sendAsync(invMsg.getPtr(), dbg);
	    changeState(agent, UaStateFactory::instance().getState(U_STATE_INCALL));
	    
        }
        break;
        case SIP_ACK:
        {
            //Generate ACK of the StatusMsg
            Sptr<StatusMsg> sMsg;
            sMsg.dynamicCast(agent.getResponse());
            assert(sMsg != 0);
            Sptr<AckMsg> ackMsg = new AckMsg(*sMsg, agent.getMyLocalIp());
	    addSelfInVia(agent, ackMsg.getPtr());
            agent.getSipTransceiver()->sendAsync(ackMsg.getPtr(), dbg);
            cpLog(LOG_DEBUG, "(%s) Sending Ack %s",
                  agent.getInstanceName().c_str(), ackMsg->encode().logData());
        }
        break;
        case SIP_CANCEL:
        {
            cpLog(LOG_WARNING, "UaStateInHold, processing SIP_CANCEL, this is incorrect");
            Sptr<SipCommand> sipCmd;
            sipCmd.dynamicCast(agent.getRequest());
            Sptr<CancelMsg> cMsg = new CancelMsg(*sipCmd);
            agent.getSipTransceiver()->sendAsync(cMsg.getPtr(), dbg);
            //Transit to End
            changeState(agent, UaStateFactory::instance().getState(U_STATE_END));
        }
        break;
        default:
        {
            cpLog(LOG_WARNING, "Wrong message type (%d) to handle in In Call, ignoring..", msg->getType());
            rv = -EINVAL;
        }
        break;
    }
    return rv;
}
