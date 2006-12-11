
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
#include <stdio.h>

#include "UaStateIdle.hxx"
#include "UaBase.hxx"
#include "UaStateFactory.hxx"
#include "cpLog.h"

using namespace Vocal;
using namespace UA;

void 
UaStateIdle::recvRequest(UaBase& agent, Sptr<SipMsg> msg) throw (CInvalidStateException&)
{
    cpLog(LOG_DEBUG, "(%s) UaStateIdle::recvRequest", agent.getInstanceName().c_str());
    if(agent.isAServer() && (msg->getType() == SIP_INVITE))
    {
        agent.saveRouteList(msg, false);
        agent.sendReplyForRequest(msg, 100); 
        ///Manipulate SDP data
        Sptr<SipSdp> sipSdp;
        sipSdp.dynamicCast(msg->getContentData(0));
        // 29/1/04 fpi
        // adding slowstart management
        if(sipSdp != 0)
            agent.setRemoteSdp(sipSdp);
        changeState(agent, UaStateFactory::instance().getState(U_STATE_S_TRYING));
    }
    else 
    {
        cpLog(LOG_WARNING, "(%s) receiving message (%s) in wrong state, ignoring", agent.getInstanceName().c_str(), msg->encode().logData());
    }
}

int
UaStateIdle::sendRequest(UaBase& agent, Sptr<SipMsg> msg, const char* dbg)
   throw (CInvalidStateException&) {
    cpLog(LOG_DEBUG, "(%s) UaStateIdle::sendRequest", agent.getInstanceName().c_str());
    if(agent.isAClient() && (msg->getType() == SIP_INVITE))
    {
        Sptr<SipCommand> sipCmd;
        sipCmd.dynamicCast(msg);
        assert(sipCmd != 0);
        string outboundProxyAddr = agent.getProxyAddr()->getIpName().convertString();
        int outboundProxyPort =  agent.getProxyAddr()->getPort();
        if (outboundProxyAddr.length() && (outboundProxyAddr != "0.0.0.0"))
        {
            agent.getSipTransceiver()->sendAsync(sipCmd, outboundProxyAddr, 
                                                 Data(outboundProxyPort), dbg);
        } 
        else
        {
           agent.getSipTransceiver()->sendAsync(sipCmd, dbg);
        } 
        changeState(agent, UaStateFactory::instance().getState(U_STATE_C_TRYING));
        return 0;
    }
    else
    {
         cpLog(LOG_WARNING, "(%s) UaStateIdle::sendRequest, asked to send msgType (%d), ignoring..",
               agent.getInstanceName().c_str(), msg->getType());
         return -1;
    }
}


int
UaStateIdle::sendStatus(UaBase& agent, Sptr<SipMsg> msg, const char* dbg) throw (CInvalidStateException&)
{
    //Do Nothing
   return -1;
}


void 
UaStateIdle::recvStatus(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    //DO nothing
}
