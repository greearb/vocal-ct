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
"$Id: SipProxyEvent.hxx,v 1.1 2004/05/01 04:15:25 greear Exp $";


#include "Sptr.hxx"
#include "Fifo.h"


namespace Vocal
{

class SipProxyEvent;
class SipTransceiver;
class CallContainer;
class CallInfo;

/** Object SipProxyEvent
<pre>
<br> Usage of this Class </br>

     It has an output fifo and callinfo.
     A event can post itself to its output fifo

   FooDosomthing()
   {

      Sptr < Fifo < Sptr < SipProxyEvent > > > myOutputFifo;
      Sptr < SipEvent > nextEvent = new SipEvent(myOutputFifo);
      if ( nextEvent != 0 )
       {
         nextEvent->setSipReceive(sipRcv);
         nextEvent->setSipStack(mySipStack);
         if(myCallLegHistory) nextEvent->setCallLeg();
         myOutputFifo->add(nextEvent);
      }
    }

</pre>
*/
class SipProxyEvent
{
   public:
      ///
      SipProxyEvent();

      /// Create given an associated fifo.
      SipProxyEvent(Sptr < Fifo < Sptr < SipProxyEvent > > > fifo);

      ///
      virtual ~SipProxyEvent();

      /** Post the given event to the fifo associated with this event.
       *  This only really makes sense if this event is newEvent.
       */
      virtual void postEvent(const Sptr < SipProxyEvent > newEvent);

      /// Post the given event to the given fifo.
      virtual void postEvent( const Sptr < SipProxyEvent > newEvent, 
                              const Sptr < Fifo < Sptr < SipProxyEvent > > > newFifo) const;

      /// Set the call info and the call container associated with this event.
      void setCallInfo(const Sptr < CallInfo > callInfo, 
                       const Sptr < CallContainer > container );

      /// Access the associated call info. May be zero if not set.
      Sptr < CallInfo > getCallInfo() const;

      /// Remove the call info and call container associated with this event.
      void removeCallInfo();

      /** Accessor to the fifo associate with this event.
       *  May be zero if not set.
       */
      Sptr < Fifo < Sptr < SipProxyEvent > > > getFifo() const;

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

      virtual string toString();

   protected:
      /// The fifo associated with this event.
      Sptr < Fifo < Sptr < SipProxyEvent > > > myFifo;

      /** The FifoEventId associated with this event. This only makes
       *  sense for events that were posted to a fifo with a delay.
       */
      FifoEventId myId;

      /// The call info associated with this event.
      Sptr < CallInfo > myCallInfo;

      /// The call container associated with this event.
      Sptr < CallContainer > myCallContainer;

      /// Sip stack associated with this event.
      Sptr < SipTransceiver > mySipStack;

   private:

      // Suppress copying
      SipProxyEvent(const SipProxyEvent &);
        
      // Suppress copying
      const SipProxyEvent & operator=(const SipProxyEvent &);
};
 
}

#endif // SIP_PROXY_EVENT_HXX
