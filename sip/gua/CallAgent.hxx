#ifndef CallAgent_hxx
#define CallAgent_hxx

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


#include "global.h"
#include <map>
#include "Sptr.hxx"
#include "BasicAgent.hxx"
#include "ControlState.hxx"
#include "UaDef.hxx"

namespace Vocal
{

namespace UA
{

class UaFacade;

/// Object to hold a thread for call-setup/teardown 
class CallAgent : public BasicAgent {

public:
   /// Create one with default values
   CallAgent(int callId, Sptr<SipMsg> sMsg, UaFacade* facade, AgentRole aRole);

   virtual ~CallAgent();

   // Release our media resources.
   virtual void freeMedia();
   
   ///
   void setState(ControlState* state) { myState = state; };


   /** @name - Server side hooks */
   //{@
   ///
   void inCall();

   ///
   void callFailed();
   
   ///
   void endCall();

   ///
   void doCancel();

   ///
   int sendBusy();
   ///
   void receivedRequest(UaBase& agent, const Sptr<SipMsg>& msg);
   ///
   void hold(UaBase& agent, const Sptr<SipMsg>& msg);
   //@}
   void reqResume( Sptr<SipMsg>& msg);


   ///
   void placeCall();
   ///
   void acceptCall();
   /// 
   void stopCall();
   ///
   int sendCancel();
   ///
   void doBye();
   ///
   void receivedStatus(UaBase& agent, const Sptr<SipMsg>& msg);
   ///
   void doResume(Sptr<SipMsg>& msg);
   
   virtual void setDeleted();

   ///
   void dohold();
   ///
   void resume();    
   
   ///
   void processHold();
   ///
   void processResume();
   void doResumeReinvite(Sptr<SipSdp> remoteSdp);
   ///
   bool isActive() const { return myActiveFlg; };

   ///Handle 401/407 challenges
   void doAuthentication(Sptr<StatusMsg> sMsg);
   
   static int getInstanceCount() { return _cnt; }

private:
   Sptr<StatusMsg> doholdresume200OKstuff(const Sptr<SipMsg>& msg,
                                          SdpSession& remoteSdp);
   void startSession(SdpSession& localSdp, SdpSession& remoteSdp);

   ControlState* myState; 

   UaFacade* facade;

   AgentRole myRole;

   bool myActiveFlg;
   bool freedMedia;

   // Not implemented.
   CallAgent();
   const CallAgent& operator =( const CallAgent& src );
   CallAgent( const CallAgent& src );

   static int _cnt;
};

}

}


#endif
