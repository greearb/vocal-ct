#ifndef CallControl_hxx
#define CallControl_hxx

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


static const char* const CallControl_hxx_Version =
    "$Id: CallControl.hxx,v 1.2 2004/06/15 00:30:11 greear Exp $";

#include "global.h"
#include <map>

#include "SipProxyEvent.hxx"
#include "SipCallLeg.hxx"
#include "BasicAgent.hxx"
#include "UaBase.hxx"

using namespace Vocal::Threads;

namespace Vocal {

namespace UA {

/**Object CallControl
   <pre>
   Main interface to handle requests in 3pcc
   </pre>
*/
class CallControl 
{
   public:
      ///
      virtual string className() { return "CallControl"; }

      ///Destructor
      virtual ~CallControl();

      ///Hook to call the controller when a request is received
      virtual void receivedRequest(UaBase& agent, const Sptr<SipMsg>& msg) { };
      ///Hook to call controller when a status is received
      virtual void receivedStatus(UaBase& agent, const Sptr<SipMsg>& msg) { };

      /**Called by the application when either a SIP, user event is
       * received. Only handles the SIP event, if it is part of an exisitng
       * call-leg. Returns true if successfully handles the event.
       */
      virtual bool processEvent(const Sptr<SipProxyEvent>& event);

      ///Thread to garbage collect call-agents when done with a call
      static void* cleanupThread(void*);

      /**Application calls to schedule a call-agent for collection
       * @param id - Indentifies the agent in callDB
       * @param timeInMs - Time after which agent will be collected
       */
      virtual void removeAgent(unsigned long id, int timeInMs=5000);

   protected:
      ///
      typedef map<int , Sptr<BasicAgent> > CallMap;

      ///
      CallControl();
      ///
      CallMap     myCallMap;
      ///
      VThread    myCleanupThread;
      ///
      bool       myShutdownFlag;
      ///
      Fifo<unsigned long> myFriedFifo;

      ///
      Mutex     myMutex;
};

}

}


#endif
