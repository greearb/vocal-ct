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


#include "UaStateInCall.hxx"
#include "UaStateFactory.hxx"
#include "SystemInfo.hxx"
#include "BasicAgent.hxx"
#include "AckMsg.hxx"
#include "SystemInfo.hxx"
#include "CancelMsg.hxx"
#include "UaBase.hxx"
#include <errno.h>


using namespace Vocal::UA;

using namespace Vocal;

void 
UaStateInCall::recvRequest(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    agent.assertNotDeleted();
    cpLog(LOG_DEBUG, "UaStateInCall::recvRequest");
    Sptr<SipCommand> sipCmd;
    sipCmd.dynamicCast(msg);
    assert(sipCmd != 0); 
    switch(msg->getType())
    {
        case SIP_INFO:
        {
            cpLog(LOG_DEBUG, "UaStateInCall, processing SIP_INFO");
            //Send 200 status msg
            agent.sendReplyForRequest(msg, 200);
            //Notify CC
            Sptr<BasicAgent> ba = agent.getControllerAgent();
            if (ba != 0) {
               ba->receivedRequest(agent, msg);
            }
        }
        break;
        case SIP_BYE:
        {
            cpLog(LOG_DEBUG, "UaStateInCall, processing SIP_BYE");
            //Send 200 
            agent.sendReplyForRequest(msg, 200);
            //Notify CC of the call end
            Sptr<BasicAgent> ba = agent.getControllerAgent();
            if (ba != 0) {
                ba->endCall();
                ba->receivedRequest(agent, msg);
            }

            //Transit to Idle
            changeState(agent, UaStateFactory::instance().getState(U_STATE_IDLE));
            cpLog(LOG_DEBUG, "UaStateInCall, done processing SIP_BYE");
        }
        break;
        case SIP_ACK:
        {
            cpLog(LOG_DEBUG, "UaStateInCall, processing SIP_ACK");
            //Save the Ack message for future BYE
            agent.setAck(msg);
            //Notify CC
            Sptr<BasicAgent> ba = agent.getControllerAgent();
            if (ba != 0) {
               ba->receivedRequest(agent, msg);
            }
        }
        break;
        case SIP_INVITE:
        {
            cpLog(LOG_DEBUG, "UaStateInCall, processing SIP_INVITE");
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
                ba->hold(agent, msg);
            }

	    changeState(agent, UaStateFactory::instance().getState(U_STATE_HOLD));
        }
        break;
        case SIP_CANCEL:
        {
            cpLog(LOG_WARNING, "UaStateInCall, processing SIP_CANCEL, this is incorrect");
            //Send 200 
            agent.sendReplyForRequest(msg, 200);
            agent.sendReplyForRequest(msg, 487);

            //Notify CC of the call end
            Sptr<BasicAgent> ba = agent.getControllerAgent();
            if (ba != 0) {
               ba->receivedRequest(agent, msg);
               ba->endCall();
            }

            //Transit to Idle
            changeState(agent, UaStateFactory::instance().getState(U_STATE_FAILURE));
        }
        break;

        case SIP_OPTIONS: {
           cpLog(LOG_DEBUG, "UaStateInCall, processing SIP_OPTIONS");

           // 26/1/04 fpi
           // avoid general reply for request, we nee to
           // set allowed
           // ! Send 200 status msg
           // ! agent.sendReplyForRequest(msg, 200);				
           Sptr<StatusMsg> infoStatusReply = new StatusMsg(*sipCmd, 200);
           infoStatusReply->setAllow(Data(UaBase::allowField), 0);
           
           cpLog(LOG_DEBUG, "(%s) sending status %s", className().c_str(),
                 infoStatusReply->encode().logData());
           agent.getSipTransceiver()->sendReply(infoStatusReply);
           
           // 26/1/04 fpi
           // we do not notify the controllerAgent,
           // because it's not interested in our reply
           // ! Notify CC
           // ! BasicAgent* ba = agent.getControllerAgent();
           // ! if (ba) {
           // !   ba->receivedRequest(agent, msg);
           // ! }
        }
        break;
        default:
            cpLog(LOG_WARNING, "Do not know what to do with message type %d:" , msg->getType());
        break;
    }
}

int
UaStateInCall::sendRequest(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    int rv = 0; /* 0 == success */
    cpLog(LOG_DEBUG, "UaStateInCall::sendRequest");
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
	    cpLog(LOG_DEBUG, "(%s) Building re-invite %s", 
		  agent.className().c_str(), invMsg->encode().logData());
            
            //Set the To tag from the previous final response
            SipTo to = invMsg->getTo();
	   
            if(to.getUser() == agent.getResponse()->getTo().getUser())
            {
		
		cpLog(LOG_DEBUG, "(%s) Building re-invite The To Tag: %s", 
		      agent.className().c_str(), agent.getResponse()->getTo().getTag().logData());
               	cpLog(LOG_DEBUG, "(%s) Building re-invite The response: %s", 
		      agent.className().c_str(), agent.getResponse()->encode().logData());
		to.setTag(agent.getResponse()->getTo().getTag());
            }
            else
            {
		cpLog(LOG_DEBUG, "(%s) Building re-invite %s", 
		      agent.className().c_str(), agent.getResponse()->getFrom().getTag().logData());
               to.setTag(agent.getResponse()->getFrom().getTag());
            }
            invMsg->setTo(to);

            SipFrom from = invMsg->getFrom();
            if(from.getUser() == agent.getResponse()->getFrom().getUser())
            {
                from.setTag(agent.getResponse()->getFrom().getTag());
            }
            else
            {
                from.setTag(agent.getResponse()->getTo().getTag());
            }
            invMsg->setFrom(from);

            cpLog(LOG_DEBUG, "(%s) Sending re-invite %s",
                  agent.className().c_str(), invMsg->encode().logData());
            agent.getSipTransceiver()->sendAsync(invMsg.getPtr());
	    changeState(agent, UaStateFactory::instance().getState(U_STATE_HOLD));
	    
        }
        break;
        case SIP_ACK:
        {
            //Generate ACK of the StatusMsg
            Sptr<StatusMsg> sMsg;
            sMsg.dynamicCast(agent.getResponse());
            assert(sMsg != 0);

            // 28/1/04 fpi
            // adding slowstart management
            Sptr<SipSdp> sipSdp;
            sipSdp.dynamicCast(msg->getContentData(0));
            agent.ackStatus(sMsg.getPtr(), sipSdp);

            Sptr<AckMsg> ackMsg = new AckMsg(*sMsg, agent.getMyLocalIp());	
            if(sipSdp != 0) {
               agent.setRemoteSdp(sipSdp);
               ackMsg->setNumContentData(0);
               ackMsg->setContentData(sipSdp.getPtr());
            }

            //addSelfInVia(ackMsg);
            //agent.getSipTransceiver()->sendAsync(ackMsg);
            cpLog(LOG_DEBUG, "(%s) Sending Ack %s", agent.className().c_str(), ackMsg->encode().logData());
        }
        break;
        case SIP_CANCEL:
        {
            cpLog(LOG_WARNING, "UaStateInCall, processing SIP_CANCEL, this is incorrect");
            Sptr<SipCommand> sipCmd;
            sipCmd.dynamicCast(agent.getRequest());
            Sptr<CancelMsg> cMsg = new CancelMsg(*sipCmd);
            agent.getSipTransceiver()->sendAsync(cMsg.getPtr());
            //Transit to End
            changeState(agent, UaStateFactory::instance().getState(U_STATE_END));
        }
        break;
        default:
        {
            cpLog(LOG_WARNING, "Wrong message type (%d) to handle in In Call, ignoring..",
                  msg->getType());
            rv = -EINVAL;
        }
        break;
    }/* switch */
    return rv;
}

void 
UaStateInCall::recvStatus(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    cpLog(LOG_DEBUG, "UaStateInCall::recvStatus");
    Sptr<StatusMsg> statusMsg;
    statusMsg.dynamicCast(msg);
    assert(statusMsg != 0);
    int statusCode = statusMsg->getStatusLine().getStatusCode();
    if (statusCode > 200) {
       //Send Ack
       Sptr<AckMsg> ackMsg = new AckMsg(*statusMsg, agent.getMyLocalIp());
       SipRequestLine& ackRequestLine = ackMsg->getMutableRequestLine();
       Sptr<SipCommand> sCommand;
       sCommand.dynamicCast(agent.getRequest());
       assert(sCommand != 0);
       addSelfInVia(agent, ackMsg.getPtr());
       ackMsg->setRouteList(agent.getRouteList());
       //Remove the top route
       ackMsg->removeRoute(0);
       ackRequestLine.setUrl(sCommand->getRequestLine().getUrl());
       agent.getSipTransceiver()->sendAsync(ackMsg.getPtr());
    }

    if (statusCode == 200) {
       if (statusMsg->getCSeq().getMethod() == INVITE_METHOD) {
          Sptr<AckMsg> ackMsg = new AckMsg(*statusMsg, agent.getMyLocalIp());
          addSelfInVia(agent, ackMsg.getPtr());
          ackMsg->setRouteList(agent.getRouteList());
          //Remove the top route
          ackMsg->removeRoute(0);
          //Sync up the sequence number
          agent.getResponse()->setCSeq(statusMsg->getCSeq());
          agent.setLocalCSeq(statusMsg->getCSeq());
       }
    }

    //Notify CC
    Sptr<BasicAgent> ba = agent.getControllerAgent();
    if (ba != 0) {
       ba->receivedStatus(agent, msg);
    }
}

int
UaStateInCall::sendStatus(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    cpLog(LOG_DEBUG, "UaStateInCall::sendStatus");
    Sptr<StatusMsg> statusMsg;
    statusMsg.dynamicCast(msg);
    assert(statusMsg != 0);
    int statusCode = statusMsg->getStatusLine().getStatusCode();
    if (statusCode == 200) {
        Sptr<SipContentData> sdpData = msg->getContentData(0);
        Sptr<SipCommand> sipCmd;
        sipCmd.dynamicCast(agent.getRequest());
        assert(sipCmd != 0);
        Sptr<StatusMsg> sendSMsg = new StatusMsg(*sipCmd, statusCode);

        if (sdpData != 0) {
            sendSMsg->setNumContentData(0);
            sendSMsg->setContentData(sdpData.getPtr());
        }

        Sptr<BaseUrl> bUrl = sipCmd->getTo().getUrl();
        Sptr<SipUrl> mUrl;
        mUrl.dynamicCast(bUrl);
        assert(mUrl != 0);

        mUrl->setHost(agent.getNatHost());
        mUrl->setPort(agent.getMySipPort());

        SipContact me("", agent.getMyLocalIp());
        me.setUrl(mUrl.getPtr());
        sendSMsg->setNumContact( 0 );
        sendSMsg->setContact( me );
        agent.getSipTransceiver()->sendReply(sendSMsg);
    }
    else if(statusCode > 200) {
        agent.sendReplyForRequest(agent.getRequest(), statusCode);
    }
    return 0;
}



