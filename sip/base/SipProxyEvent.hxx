#ifndef SIP_PROXY_EVENT_HXX
#define SIP_PROXY_EVENT_HXX

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

static const char* const SipProxyEvent_hxx_Version = 
"$Id: SipProxyEvent.hxx,v 1.7 2004/06/20 07:09:38 greear Exp $";


#include "Sptr.hxx"
#include <BugCatcher.hxx>
#include <list>
#include <misc.hxx>

namespace Vocal
{

class SipTransceiver;
class CallContainer;
class CallInfo;

class SipProxyEvent: public BugCatcher {
public:
   ///
   SipProxyEvent(uint64 runTimerMs = 0);

   ///
   virtual ~SipProxyEvent();

   /// Set the call info and the call container associated with this event.
   void setCallInfo(const Sptr < CallInfo > callInfo, 
                    const Sptr < CallContainer > container );

   /// Access the associated call info. May be zero if not set.
   Sptr < CallInfo > getCallInfo() const;

   /// Remove the call info and call container associated with this event.
   void removeCallInfo();

   /** Accessor to the call container for this event. 
    *  May be zero if not set.
    */
   Sptr < CallContainer > getCallContainer() const;

   ///
   void setSipStack( const Sptr < SipTransceiver > sipStack );

   ///
   const Sptr < SipTransceiver > getSipStack();

   // The name of the extending class.
   virtual const char* const name() const = 0;

   // To help with casting...
   virtual bool isTimerEvent() const { return false; }
   virtual bool isSipEvent() const { return false; }

   // Run no sooner than this, in Miliseconds.
   uint64 getRunTimer() const { return runTimer; }

   virtual string toString() const;

protected:

   /// The call info associated with this event.
   Sptr < CallInfo > myCallInfo;

   /// The call container associated with this event.
   Sptr < CallContainer > myCallContainer;

   /// Sip stack associated with this event.
   Sptr < SipTransceiver > mySipStack;

   // Should not process the event before this time (in miliseconds)
   uint64 runTimer;

private:

   // Suppress copying
   SipProxyEvent(const SipProxyEvent &);
        
   // Suppress copying
   const SipProxyEvent & operator=(const SipProxyEvent &);
};

// We want the lower time to be first in the heap, used to sort
// event queues.
struct SipProxyEventComparitor {
   bool operator()(Sptr<SipProxyEvent> a, Sptr<SipProxyEvent> b) {
      return (b->getRunTimer() < a->getRunTimer());
   }
};
 
}

#endif // SIP_PROXY_EVENT_HXX
