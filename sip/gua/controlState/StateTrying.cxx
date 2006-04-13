
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

#include "StateTrying.hxx"
#include "CallAgent.hxx"
#include "ControlStateFactory.hxx"
#include "cpLog.h"

using namespace Vocal;
using namespace Vocal::UA;

void 
StateTrying::cancel(CallAgent& agent) throw (CInvalidStateException&)
{
    cpLog(LOG_DEBUG, "StateTrying::cancel");
    changeState(agent, ControlStateFactory::instance().getState(TEAR_DOWN));
}


int
StateTrying::end(CallAgent& agent) throw (CInvalidStateException&)
{
    cpLog(LOG_DEBUG, "StateTrying::end");
    int rv;
    //Send Cancel 
    if ((rv = agent.sendCancel()) < 0) {
       // Could not send the cancel, need to transition to a failed state
       changeState(agent, ControlStateFactory::instance().getState(TEAR_DOWN));
    }
    return rv;
}

void 
StateTrying::ringing(CallAgent& agent) throw (CInvalidStateException&)
{
    cpLog(LOG_DEBUG, "StateTrying::ringing");
    changeState(agent, ControlStateFactory::instance().getState(UAC_RINGING));
}

void 
StateTrying::inCall(CallAgent& agent) throw (CInvalidStateException&)
{
    cpLog(LOG_DEBUG, "StateTrying::inCall");
    changeState(agent, ControlStateFactory::instance().getState(IN_CALL));
}

void 
StateTrying::recvStatus(CallAgent& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    Sptr<StatusMsg> statusMsg;
    statusMsg.dynamicCast(msg);
    assert(statusMsg != 0);
    int statusCode = statusMsg->getStatusLine().getStatusCode();
    cpLog(LOG_DEBUG, "StateTrying::recvStatus[%s]", statusMsg->encode().logData());
    if(statusCode == 200) {
        agent.inCall();
    }
    else if ((statusCode == 401) || (statusCode == 407)) {
        agent.doAuthentication(statusMsg);
    }
    else if((statusCode == 302) && (msg->getCSeq().getMethod() == INVITE_METHOD) ) {
        //TODO, for now do nothing
    }
    else if(statusCode > 200) {
        agent.doCancel();
    }
    else if((statusCode == 180)||
            (statusCode == 183)) {
        ringing(agent);
    }
}



