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


static const char* const CallControl_cxx_Version =
    "$Id: CallControl.cxx,v 1.4 2004/10/29 07:22:35 greear Exp $";

#pragma warning (disable: 4786)

#include "SipEvent.hxx" 
#include "CallTimerEvent.hxx" 
#include "CallDB.hxx" 
#include "CallControl.hxx" 

using namespace Vocal;
using namespace UA;

bool 
CallControl::processEvent(const Sptr<SipProxyEvent>& event) {
    cpLog(LOG_DEBUG, "CallControl::processEvent");
    bool eventHandled = false;
    //Lets handle the sip-event here, other custom events can 
    //be handled in the derived classes
    Sptr<SipEvent> sipEvent;
    sipEvent.dynamicCast(event);
    if(sipEvent != 0) {
        cpLog(LOG_DEBUG, "Handling SIP event");
        //get the callleg and lookup in the Call database to
        //see if callleg already exists.If so get the UserAgent and
        //invoke the action on it.
        Sptr<SipMsg> sipMsg = sipEvent->getSipMsg();
        assert(sipMsg != 0);
        SipCallLeg cLeg(sipMsg->getFrom(), sipMsg->getTo(), sipMsg->getCallId(),
                        sipMsg->getLocalIp());
        Sptr<MultiLegCallData> mData = CallDB::instance().getMultiLegCallData(cLeg);
        if (mData != 0) {
            mData->assertNotDeleted();
            //Call Data found
            Sptr<UaBase> agent;
            agent.dynamicCast(mData->getCallLeg(cLeg));
            if (agent != 0) {
                agent->assertNotDeleted();
                agent->receivedMsg(sipMsg);
            }
            eventHandled = true;
        }
        else {
           cpLog(LOG_DEBUG_STACK, "Could not find MultiLegCallData, cLen: %s\n",
                 cLeg.toString().c_str());
        }
    }
    cpLog(LOG_DEBUG, "CallControl::processEvent returns");
    return eventHandled;
}


CallControl::~CallControl() {
   // Nothing to do
}

CallControl::CallControl() {
   // Nothing to do
}

void CallControl::removeAgent(unsigned long id) {
   myCallMap.erase(id);
}
