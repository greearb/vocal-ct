#ifndef SIP_EVENT_HXX
#define SIP_EVENT_HXX

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


static const char* const SipEvent_hxx_Version = 
    "$Id: SipEvent.hxx,v 1.4 2004/05/27 04:32:18 greear Exp $";



#include <vector>

#include "SipMsgQueue.hxx"
#include "SipCallLeg.hxx"
#include "SipProxyEvent.hxx"


namespace Vocal
{

class SipCommand;
class InviteMsg;
class SipMsg;


/** Object SipEvent
<pre>
<br> Usage of this Class </br>

    SipEvent is a SipProxyEvent. SipThread object creates a SipEvent
    on receiving a SipMsg. SipEvent gets posted to an output fifo.

</pre>
*/
class SipEvent : public SipProxyEvent
{
public:

   /** Convenience typedef.
    */
   typedef vector < Sptr < SipMsg > > CallLegVector;


   /** Create the sip event.
    */
   SipEvent();

   /** Virtual destructor.
    */
   virtual ~SipEvent();

   virtual bool isSipEvent() const { return true; }

   /** The name of the class.
    */
   virtual const char* const name() const {
      return "SipEvent";
   }

   
   /** Set the sip message associated with this event. Updates the 
    *  call leg.
    */
   void setSipMsg(const Sptr < SipMsg > sipMsg);

   //
   void setCallLeg();

   /** Returns the associated sip message.
    */
   const Sptr < SipMsg > getSipMsg() const;


   /** Set the sip message queue associated with this event.
    *  It also updates the sip message.
    *  A copy is made, so do not assume modifying sipRcv after
    *  this call will change the SipEvent class in any way.
    */
   void setSipMsgQueue(const SipMsgQueue& sipRcv );


   /** Get the sip message queue associated with this event.
    */
   SipMsgQueue& getSipMsgQueue();


   /** Returns the INVITE associated with the event, or 0 if none exists
    */
   const Sptr < InviteMsg > getInvite() const;

   /** Returns the SIP request associated with the event, or 0 if none exists
    */
   const Sptr < SipCommand > getCommand() const;

   /** Takes a Command and returns the corresponding command which is to 
    *  be cancelled, or 0 if none exists
    */
   const Sptr < SipCommand > 
   getPendingCommand( Sptr < SipCommand > SipCommand ) const;

   //
   Sptr < SipCallLeg > getSipCallLeg() const;

   const string& getLocalIp() const { return getSipCallLeg()->getLocalIp(); }

   /**
    */
   void removeCall();

   virtual string toString() const;
   
private:


   /** Sip message associated with this event.
    */
   Sptr < SipMsg > mySipMsg;


   /** Sip message queue associate with this event. May be updated
    *  during the lifetime of an event.
    */
   SipMsgQueue mySipMsgQueue;


   /** Sip call leg associated with this event. May be updated during
    *  this lifetime of an event.
    */
   Sptr < SipCallLeg > mySipCallLeg;

        
   /** Suppress copying
    */
   SipEvent(const SipEvent &);
        
   
   /** Suppress copying
    */
   const SipEvent & operator=(const SipEvent &);
};
 
}

#endif // SIP_EVENT_HXX
