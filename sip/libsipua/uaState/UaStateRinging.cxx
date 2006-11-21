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



#include "UaStateRinging.hxx"
#include "UaStateFactory.hxx"
#include "BasicAgent.hxx"
#include "StatusMsg.hxx"
#include "AckMsg.hxx"
#include "SipVia.hxx"
#include "CancelMsg.hxx"
#include "SipCommand.hxx"

using namespace Vocal::UA;

void 
UaStateRinging::recvStatus(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    Sptr<StatusMsg> statusMsg;
    statusMsg.dynamicCast(msg);
    assert(statusMsg != 0);
    int statusCode = statusMsg->getStatusLine().getStatusCode();
    cpLog(LOG_DEBUG, "UaStateRinging::recvStatus:%d", statusCode);
    if(statusCode == 200 )
    {
        //Received a request, get the received IP and
        //Set it in SDP
        Data receivedIp = msg->getReceivedIPName();
//        assert(strstr(receivedIp.c_str(), ":") == NULL);
        agent.fixSdpForNat(msg, receivedIp);

        //Save final response for future BYE
        agent.setResponse(msg);
        agent.saveRouteList(msg, true);
        Sptr<SipSdp> sdp;
        sdp.dynamicCast(msg->getContentData(0));

        agent.setRemoteSdp(sdp);
        agent.getResponse()->setCSeq(statusMsg->getCSeq());
        agent.setLocalCSeq(statusMsg->getCSeq());
        //Notify CC
        Sptr<BasicAgent> ba = agent.getControllerAgent();
        if (ba != 0) {
           ba->receivedStatus(agent, msg);
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
            agent.getSipTransceiver()->sendAsync(ackMsg.getPtr(), "Uas Ringing, ack");
            cpLog(LOG_INFO, "Sent Ack for status (%d), going to idle state:%s",
                  statusCode, ackMsg->encode().logData());
        }

        //Notify CC
        Sptr<BasicAgent> ba = agent.getControllerAgent();
        if (ba != 0) {
           ba->receivedStatus(agent, msg);
        }

        //Transit to Idle
        changeState(agent, UaStateFactory::instance().getState(U_STATE_IDLE));
    }
}

int
UaStateRinging::sendStatus(UaBase& agent, Sptr<SipMsg> msg, const char* dbg)
                 throw (CInvalidStateException&)
{
    Sptr<StatusMsg> statusMsg;
    statusMsg.dynamicCast(msg);
    assert(statusMsg != 0);
    int statusCode = statusMsg->getStatusLine().getStatusCode();
    if(statusCode < 200)
    {
        cpLog(LOG_WARNING, "UaStateRinging::Why are we receiving < 200 , ignoring..");
        return -1;
    }

    if(((statusCode > 200) && (statusCode < 400)) || (statusCode > 500))
    {
        cpLog(LOG_WARNING, "UaStateRinging::Received %d", statusCode);
        Sptr<BasicAgent> ba = agent.getControllerAgent();
        if (ba != 0) {
           ba->endCall();
        }
        //Transit to idle
        changeState(agent, UaStateFactory::instance().getState(U_STATE_IDLE));
        return 0;
    }
	 
    // 13/1/04 fpi
    // bugFix Bugzilla 772
    if ((statusCode >= 400) && (statusCode < 500)) {		 
       cpLog(LOG_WARNING, "(%s)UaStateRinging::Received %d",
             agent.className().c_str(), statusCode);
       
       // Send Status Message
       Sptr<SipCommand> sipCmd;
       sipCmd.dynamicCast(agent.getRequest());
       assert(sipCmd != 0);
       Sptr<StatusMsg> sendSMsg = new StatusMsg(*sipCmd, statusCode);
       sendSMsg->setNumContentData(0);
		 
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
       
       agent.setCallLegState(C_LIVE);
       agent.setResponse(sendSMsg.getPtr());

       //Notify CC the call failed.
       Sptr<BasicAgent> ba = agent.getControllerAgent();
       if (ba != 0) {
          ba->callFailed();
       }

       //Transit to failure
       changeState(agent, UaStateFactory::instance().getState(U_STATE_FAILURE));
       return 0;
    }

    //Only handle status code 200
    //Send status message
    Sptr<SipSdp> sdpData;
    sdpData.dynamicCast(msg->getContentData(0));


    //Received a request, get the received IP and
    //Set it in SDP
    //Data receivedIp = msg->getReceivedIPName();
    //agent.fixSdpForNat(msg, receivedIp);

    Sptr<SipCommand> sipCmd;
    sipCmd.dynamicCast(agent.getRequest());
    assert(sipCmd != 0);
    Sptr<StatusMsg> sendSMsg = new StatusMsg(*sipCmd, statusCode);
    sendSMsg->setNumContentData(0);
    if(sdpData != 0)
    {
        sendSMsg->setContentData(sdpData.getPtr());
        agent.setLocalSdp(sdpData); 
    }
    //// Find a nice way to do it

    Sptr<BaseUrl> bUrl = sipCmd->getTo().getUrl();
    Sptr<SipUrl> mUrl;
    mUrl.dynamicCast(bUrl);

    if(mUrl == 0)
    {
	// get out of here because we cannot send appropriately.
        cpLog(LOG_WARNING, "To does not contain sip: URL");
        //Transit to idle
        changeState(agent, UaStateFactory::instance().getState(U_STATE_IDLE));
	throw CInvalidStateException("does not contain sip: URL", 
				     __FILE__, __LINE__, 0);
        return 0;
    }

    assert(mUrl != 0);
    mUrl->setHost(agent.getNatHost());
    mUrl->setPort(agent.getMySipPort());

    SipContact me("", agent.getMyLocalIp());
    me.setUrl(mUrl.getPtr());
    sendSMsg->setNumContact( 0 );
    sendSMsg->setContact( me );


    cpLog(LOG_DEBUG, "(%s) sending status %s", className().c_str(), sendSMsg->encode().logData());
    agent.getSipTransceiver()->sendReply(sendSMsg, dbg);
    agent.setResponse(sendSMsg.getPtr());
    //Notify CC to start the call monitor
    Sptr<BasicAgent> ba = agent.getControllerAgent();
    if (ba != 0) {
       ba->inCall();
    }
    agent.setCallLegState(C_LIVE);
    agent.setResponse(sendSMsg.getPtr());
    //Transit to inCall
    changeState(agent, UaStateFactory::instance().getState(U_STATE_INCALL));
    return 0;
}


void 
UaStateRinging::recvRequest(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
     if((msg->getType() == SIP_CANCEL) ||
        (msg->getType() == SIP_BYE))
     {
         agent.sendReplyForRequest(msg, 200);
         if(msg->getType() == SIP_CANCEL)
         {
            //Send status 487
            agent.sendReplyForRequest(agent.getRequest(), 487);
            
            // 13/1/04 fpi
            // BugFix Bugzilla 773
            // You cannot doCancel() here, otherwise callLeg will
            // be deleted and transaction will be not completed
            // if(agent.getControllerAgent())
            //		agent.getControllerAgent()->doCancel();
         }
         agent.setCallLegState(CLS_NONE);
         //Notify CC
         Sptr<BasicAgent> ba = agent.getControllerAgent();
         if (ba != 0) {
            ba->receivedRequest(agent, msg);
         }
         //Transit to Failure
         changeState(agent, UaStateFactory::instance().getState(U_STATE_FAILURE));
    }
}


int UaStateRinging::sendRequest(UaBase& agent, Sptr<SipMsg> msg, const char* dbg)
   throw (CInvalidStateException&) {

   //Some UAs may send BYE instead of CANCEL in Ringing
   if ((msg->getType() == SIP_CANCEL) ||
       (msg->getType() == SIP_BYE)) {
      Sptr<SipCommand> sipCmd;
      agent.setCallLegState(CLS_NONE);
      sipCmd.dynamicCast(agent.getRequest());
      Sptr<CancelMsg> cMsg = new CancelMsg(*sipCmd);
      cpLog(LOG_DEBUG, "Sending cancel:%s" , cMsg->encode().logData());
      agent.getSipTransceiver()->sendAsync(cMsg.getPtr(), dbg);
      //Transit to Failure
      changeState(agent, UaStateFactory::instance().getState(U_STATE_FAILURE));
      return 0;
   }
   return -EINVAL;
}//sendRequest
