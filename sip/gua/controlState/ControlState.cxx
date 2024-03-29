
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

#include "ControlState.hxx"
#include "cpLog.h"
#include "CallAgent.hxx"

using namespace Vocal;
using namespace Vocal::UA;

void 
ControlState::error(const string& errMsg)
{
    char buf[256];
    sprintf(buf, "****Invalid state transition, %s", errMsg.c_str());
    // throw CInvalidStateException(buf, __FILE__, __LINE__);
    //cpLog(LOG_ALERT, buf);
}

void
ControlState::acceptCall(CallAgent& agent)
{
    error(className()+"::incomingCall");
}

void
ControlState::makeCall(CallAgent& agent)
{
    error(className()+"::makeCall");
}

void 
ControlState::fail(CallAgent& agent)
{
    error(className()+"::fail");
}

void
ControlState::inCall(CallAgent& agent)
{
    error(className() + "::inCall");
}

void
ControlState::inHold(CallAgent& agent)
{
    //error(className() + "::bye");
    cpLog(LOG_DEBUG, "IN HOLD STATE\r\n");
}

void
ControlState::bye(CallAgent& agent)
{
    error(className() + "::bye");
}

void
ControlState::cancel(CallAgent& agent)
{
    error(className() + "::cancel");
}

void
ControlState::ringing(CallAgent& agent)
{
    error(className() + "::ringing");
}

int
ControlState::end(CallAgent& agent)
{
    error(className() + "::end");
    return -1;
}


void 
ControlState::recvReq(CallAgent& agent, Sptr<SipMsg> msg)
{
    error(className() + "::recvReq");
}

void 
ControlState::recvStatus(CallAgent& agent, Sptr<SipMsg> msg)
{
    error(className() + "::recvStatus");
}


void ControlState::changeState(CallAgent& agent, ControlState* newState) {
   cpLog(LOG_INFO, "ControlState::ChangeState, agent: %p  from (%s) -> (%s)",
         &agent, className().c_str(), newState->className().c_str());
   agent.setState(newState);
}

