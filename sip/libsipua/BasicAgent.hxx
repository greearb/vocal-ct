#ifndef BasicAgent_hxx
#define BasicAgent_hxx

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
#include <string>
#include "UaBase.hxx"
#include <BugCatcher.hxx>

class SipMsg;

namespace Vocal {

namespace UA {

/**Object BasicAgent
   <pre>
   <br> Usage of this Class </br>

   Abstract base class for interfacing with Call controller.
   BasicAgent is the main driver for call-setup and terdown.
   One can derive from BasicAgent to enforce any call-setup
   and tear-down characteristics.

   </pre>
*/
class BasicAgent : public BugCatcher {
public:
   ///
   BasicAgent(unsigned long authId, const string& className);
   
   BasicAgent( const BasicAgent& src );
   
   virtual ~BasicAgent();
   
   ///
   const string& className() const { return class_name; }
   
   ///
   void placeCall();
   ///
   virtual void inCall() = 0;
   
   ///
   virtual void callFailed()=0;
   ///
   virtual void doBye() = 0;
   ///
   virtual void doCancel() = 0;
   ///
   virtual void dohold() { };
   ///
   virtual void endCall()=0;
   ///
   virtual void holdAndResumeReinvite(UaBase& agent, const Sptr<SipMsg>& msg) { };
   ///
   virtual void reqResume(Sptr<SipMsg>& msg) { };
   ///
   virtual void timerExpired() { };

   ///
   unsigned long getId() const { return myAuthId; };
   
   ///
   Sptr<UaBase> getInvokee() const { return myInvokee; }
   ///
   //void setInvokee(const Sptr<UaBase>& userAgent) { myInvokee = userAgent; };
   ///
   virtual void setDeleted() = 0;
   
   ///
   virtual void receivedRequest(UaBase& agent, const Sptr<SipMsg>& msg, const char* dbg)=0;
   ///
   virtual void receivedStatus(UaBase& agent, const Sptr<SipMsg>& msg, const char* dbg)=0;
   
   static int getInstanceCount() { return _cnt; }
   
protected:
   ///
   unsigned long myAuthId;
   ///
   Sptr<UaBase> myInvokee;
   
private:
   static int _cnt;
   BasicAgent(); //restrict 
   const BasicAgent& operator =( const BasicAgent& src );

   string class_name;
   
   //static int _cnt;
};


}

}


#endif
