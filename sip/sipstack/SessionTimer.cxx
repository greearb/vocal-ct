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

static const char* const SessionTimer_cxx_version =
    "$Id: SessionTimer.cxx,v 1.4 2004/06/02 20:23:10 greear Exp $";

#include "global.h"
#include "SessionTimer.hxx"
#include "VTime.hxx"

using namespace Vocal;

SessionTimer* SessionTimer::mInstance=0;
    
SessionTimer& 
SessionTimer::instance()
{
    assert(mInstance);
    return (*mInstance);
}

SessionTimer& 
SessionTimer::instance(Sptr<SipTransceiver> tranceiver)
{
    assert(mInstance == 0);
    if(mInstance == 0)
    {
        mInstance = new SessionTimer(tranceiver);
    }
    return (*mInstance);
}

#warning "Implement non-threaded SipTcpConnection implementation."
SessionTimer::SessionTimer(Sptr<SipTransceiver> tranceiver)
   : mTransceiver(tranceiver),
     mShutdown(false)
{
   // TODO
}

void 
SessionTimer::destroy()
{
    delete SessionTimer::mInstance;
    SessionTimer::mInstance = 0;
}
SessionTimer::~SessionTimer()
{
    mShutdown = true;
    //mSessionDataFifo.addDelayMs(0,0); //TODO
}

void 
SessionTimer::processResponse(StatusMsg& sMsg)
{
    int status = sMsg.getStatusLine().getStatusCode();
    if(status == 200)
    {
        SipCallLeg cLeg = sMsg.computeCallLeg();
        cpLog(LOG_DEBUG,"SessionTimer processing response for callLeg (%s)", cLeg.encode().logData() );
        //LOcate the session data
        assert(mSessionDataMap.count(cLeg));
        int delta = sMsg.getSessionExpires().getDelta().convertInt();
        Sptr<VSessionData> sData = mSessionDataMap[cLeg];
        if(sData->mySessionContext == VS_SEND)
        {
            if(sData->myDelta != delta)
            {
                cpLog(LOG_DEBUG,"Timer (%d)s value in INVITE does not match the one (%d)s in Status, adjusting.. for callLeg (%s)", sData->myDelta, delta, cLeg.encode().logData() );
                //Set the new session interval received in the response
                //mSessionDataFifo.cancel(sData->myTimerId); TODO
                sData->myDelta = delta;
                int ts = sData->myDelta /2;
                //sData->myTimerId = mSessionDataFifo.addDelayMs(sData,ts * 1000); TODO
            }
        }
        VTime tm;
        sData->myStartTime = tm.getSeconds();
    }
    else 
    {
        cpLog(LOG_INFO, "Unexpected status message (%d) in SessionTimer", status);
    }
}
   
bool 
SessionTimer::timerExists(const InviteMsg& iMsg)
{
    SipCallLeg cLeg = iMsg.computeCallLeg();
    return((mSessionDataMap.count(cLeg)) ? true : false);
}

void
SessionTimer::startTimerFor(const InviteMsg& iMsg, Sptr<VSessionData> sData)
{
    SipCallLeg cLeg = iMsg.computeCallLeg();
    if(mSessionDataMap.count(cLeg))
    {
        cpLog(LOG_DEBUG,"Timer exists for (%s) for callLeg (%s) for (%d)s", (sData->mySessionContext == VS_SEND)? "SENDER": "RECEIVER", cLeg.encode().logData(), sData->myDelta);
         return;
    }
    cpLog(LOG_DEBUG,"Adding timer for (%s) for callLeg (%s) for (%d)s", (sData->mySessionContext == VS_SEND)? "SENDER": "RECEIVER", cLeg.encode().logData(), sData->myDelta);
    VTime tm;
    sData->myStartTime = tm.getSeconds();
    //Make a copy of Inviter message that will be reused when sending 
    //Re-Invite
    sData->myInviteMsg = new InviteMsg(iMsg);
    mSessionDataMap[cLeg] = sData;
    int ts = sData->myDelta /2;
    //sData->myTimerId = mSessionDataFifo.addDelayMs(sData,ts * 1000); //TODO
}

void 
SessionTimer::startTimerFor(const InviteMsg& iMsg, long sessionInterval, VSessionContext ct)
{
    Sptr<VSessionData>  sData = new VSessionData;
    sData->mySessionContext = ct;
    sData->myDelta = sessionInterval;
    startTimerFor(iMsg, sData);
}

void 
SessionTimer::startTimerFor(const StatusMsg& sMsg, const string& local_ip)
{
    InviteMsg iMsg(sMsg, local_ip);
    if(!sMsg.getSessionExpires().isEmpty())
    {
        iMsg.setSessionExpires(sMsg.getSessionExpires(), local_ip);
    }
    if(!sMsg.getMinSE().isEmpty())
    {
        iMsg.setMinSE(sMsg.getMinSE(), local_ip);
    }
    Sptr<VSessionData>  sData = new VSessionData;
    sData->mySessionContext = VS_SEND;
    sData->myDelta = sMsg.getSessionExpires().getDelta().convertInt();
    startTimerFor(iMsg, sData);

}

#warning "Port to non-threaded model."
#if 0
void* SessionTimer::processThreadWrapper(void *p)
{
    SessionTimer* self = static_cast<SessionTimer*>(p);
    while(1)
    {
        Sptr<VSessionData> sData = self->mSessionDataFifo.getNext();
        if(self->mShutdown) {
            break;
        }
        VTime tm;
        if((long)tm.getSeconds() >= (sData->myStartTime + sData->myDelta))
        {
            SipCallLeg cLeg = sData->myInviteMsg->computeCallLeg();
            cpLog(LOG_INFO, "Session expired for call-leg (%s)", cLeg.encode().logData()); 
            (self->myCallbackFunc)(cLeg);
        }
        else
        {
            //Send the re-INVITE and schedule the timer again
            //if refresher 
            if(sData->mySessionContext == VS_SEND)
            {
                self->sendInvite(sData);
            }
            int ts = sData->myDelta /2;
            sData->myTimerId = self->mSessionDataFifo.addDelayMs(sData,ts * 1000);
        }
    }
    return( (void*)0);
}
#endif

void
SessionTimer::sendInvite(Sptr<VSessionData> sData) {
   cpLog(LOG_DEBUG, "Sending re-invite for call-leg (%s)",
         sData->myInviteMsg->computeCallLeg().encode().logData());
   SipCSeq cSeq = sData->myInviteMsg->getCSeq();
   cSeq.incrCSeq();
   sData->myInviteMsg->setCSeq(cSeq); 
   Sptr<SipCommand> sc;
   sc.dynamicCast(sData->myInviteMsg);
   mTransceiver->sendAsync(sc);
}
