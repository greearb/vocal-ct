
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

#include "UaState.hxx"
#include "UaBase.hxx"
#include "cpLog.h"
#include "SipVia.hxx"

using namespace Vocal;
using namespace Vocal::UA;


UaState::UaState(const char* className)
      : _className(className) {
   // Nothing more to do at this point
}

void 
UaState::error(const string& errMsg)
{
    char buf[256];
    sprintf(buf, "****Invalid state transition, %s", errMsg.c_str());
    cpLog(LOG_ALERT, buf);
    ///throw CInvalidStateException(buf, __FILE__, __LINE__, 0);
}

void
UaState::info(const string& msg)
{
    char buf[256];
    sprintf(buf, "****Information, %s", msg.c_str());
    cpLog(LOG_ALERT, buf);
}


void 
UaState::recvRequest(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    error(className()+"::recvRequest");
}

int UaState::sendRequest(UaBase& agent, Sptr<SipMsg> msg, const char* dbg)
                 throw (CInvalidStateException&)
{
    error(className()+"::sendRequest");
    return -1;
}


void 
UaState::recvStatus(UaBase& agent, Sptr<SipMsg> msg)
                 throw (CInvalidStateException&)
{
    error(className()+"::recvStatus");
}

int
UaState::sendStatus(UaBase& agent, Sptr<SipMsg> msg, const char* dbg)
                 throw (CInvalidStateException&)
{
    error(className()+"::sendStatus");
    return -1;
}

void UaState::changeState(UaBase& agent, Sptr<UaState> newState) {
   cpLog(LOG_INFO, "UaState::setState from (%s) -> (%s)",
         className().c_str(), newState->className().c_str());
   agent.setState(newState);
}

void 
UaState::addSelfInVia(UaBase& agent, Sptr<SipCommand> cmd)
{
    cmd->flushViaList();
    SipVia via("", agent.getMyLocalIp());
    via.setHost(agent.getMyLocalIp());
          
    via.setPort(agent.getMySipPort());
    if(agent.getTransport() == 2) //TCP
    {
        via.setTransport("TCP");
    }
    cmd->setVia(via);
}
