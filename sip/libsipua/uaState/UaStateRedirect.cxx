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



static const char* const UaStateRedirect_cxx_Version = "";

#include "UaStateRedirect.hxx"
#include "UaStateFactory.hxx"
#include "BasicAgent.hxx"
#include "AckMsg.hxx"
#include "CancelMsg.hxx"

using namespace Vocal::UA;
using Vocal::UA::UaStateRedirect;

int
UaStateRedirect::sendRequest(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    cpLog(LOG_DEBUG, "UaStateRedirect::SendRequest");
    return 0; /** TODO:  Maybe we should send something here, or not
               * be here at all??? */
}

void 
UaStateRedirect::recvStatus(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    Sptr<StatusMsg> statusMsg;
    statusMsg.dynamicCast(msg);
    assert(statusMsg != 0);
    int statusCode = statusMsg->getStatusLine().getStatusCode();
    cpLog(LOG_DEBUG, "UaStateRedirect::recvStatus:%d", statusCode);
    if(statusCode == 200 )
    {
        //Save final response for future BYE
        agent.setResponse(msg);
        agent.saveRouteList(msg, true);
        Sptr<SipSdp> sdp;
        sdp.dynamicCast(msg->getContentData(0));
        agent.setRemoteSdp(sdp);

        //Notify CC
        if(agent.getControllerAgent())
            agent.getControllerAgent()->receivedStatus(agent, msg);
        agent.setCallLegState(C_LIVE);
        //Transit to Incall
        changeState(agent, UaStateFactory::instance().getState(U_STATE_INCALL));
    }
    else if(statusCode > 200)
    {
        if(statusCode != 408)
        {
            //Send ACK message
            Sptr<AckMsg> ackMsg = new AckMsg(*statusMsg, agent.getMyLocalIp());
            SipRequestLine& ackRequestLine = ackMsg->getMutableRequestLine();
            Sptr<SipCommand> sCommand;
            sCommand.dynamicCast(agent.getRequest());
            assert(sCommand != 0);
	    addSelfInVia(agent, ackMsg.getPtr());
            ackRequestLine.setUrl(sCommand->getRequestLine().getUrl());
            agent.getSipTransceiver()->sendAsync(ackMsg.getPtr());
            cpLog(LOG_INFO, "Sent Ack for status (%d), going to idle state:%s",
                  statusCode, ackMsg->encode().logData());
        }
        //Transit to Idle
        changeState(agent, UaStateFactory::instance().getState(U_STATE_IDLE));

        //Notify CC
        if(agent.getControllerAgent())
            agent.getControllerAgent()->receivedStatus(agent, msg);
    }
    else
    {
        //For 1xx
        //Notify CC
        if((statusCode >= 180))
        {
            //Transit to ringing
            changeState(agent, UaStateFactory::instance().getState(U_STATE_RINGING));
        }
        if(agent.getControllerAgent())
            agent.getControllerAgent()->receivedStatus(agent, msg);
    }
}


