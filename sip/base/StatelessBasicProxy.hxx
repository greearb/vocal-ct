#ifndef STATELESS_BASIC_PROXY_HXX
#define STATELESS_BASIC_PROXY_HXX

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


static const char* const StatelessBasicProxy_hxx_Version =
    "$Id: StatelessBasicProxy.hxx,v 1.4 2005/03/03 19:59:49 greear Exp $";


#include "Data.hxx"
#include "Operator.hxx"
#include "SipTransceiver.hxx"
#include "cpLog.h"
#include <HeartbeatThread.hxx>
#include "BaseProxy.hxx"

namespace Vocal
{

class SipThread;

/** Object StatelessBasicProxy
<pre>
<br> Usage of this Class </br>

     This class creates a  call container, callprocessing fifo, sipstack,
     worker and sip threads. StatelessBasicProxy::run method starts the worker
     and sip threads. It differs from the BasicProxy as it does not use
     Builder->feature->State->Operator framework and directly works with
     Operators.This lightwight impementation is suitable for SIP proxy.

     Create the proxy given the sip port (which defaults
     to 5060), the application name (which defaults to unknown).
     The boolean filteron indicates whether to createg a filtering sip
     transceiver or not (defaults to true). The boolean nat indicates
     whether the sip transceiver should be created to handle network
     address translation (defaults to false).

     StatelessBasicProxy( unsigned short          defaultSipPort = 5060,
                             Data                    applName = "unknown",
                             bool                    filteron = true,
                             bool                    nat = false,
                             SipAppContext           cContext =
                             APP_CONTEXT_GENERIC);

Exmaple:

    void FooSIPproxy::run()
    {
      StatelessBasicProxy thefooSIPproxy(mySipPort,
                           "FooSIPproxy", filter, nat , APP_CONTEXT_PROXY);

      Now,  ADD OPERATORS

        Sptr < FooOpAck > opAck           = new FooOpAck;
        thefooSIPproxy.addOperator( opAck );

        Sptr < FooOpBye > opBye           = new FooOpBye;
        thefooSIPproxy.addOperator( opBye );
        .
        .
        .

        Start Threads

        thefooSIPproxy.run();
   }


</pre>
 */
class StatelessBasicProxy: public BaseProxy
{
public:


   /** Explained in Usage of this Class section.
    * local_ip may be "", in which case we'll use the default IP and/or bind
    *  to all interfaces.
    * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
    */
   StatelessBasicProxy(uint16 tos, uint32 priority,
                       const string&            local_ip,
                       const string&            local_dev_to_bind_to,
                       ServerType              myType,
                       unsigned short          defaultSipPort = 5060,
                       Data                    applName = "unknown", 
                       bool                    filteron = true, 
                       bool                    nat = false,
                       SipAppContext           cContext = APP_CONTEXT_GENERIC);


   /** Virtual destructor
    */
   virtual ~StatelessBasicProxy();


   void addOperator(const Sptr < Operator > &   op) {
      cpLog(LOG_DEBUG, "Operator %s" , op->name());
      myOperators.push_back(op);
   }


   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);

   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

   // Call this before trying to delete this guy, or circular
   // smart-pointer references will cause memory leaks.
   virtual void clear();

   virtual void startHeartbeat(); //start sending
   virtual void stopHeartbeat(); //stop sending

   ///
   virtual void process(Sptr < SipProxyEvent > event);



protected:
   /** Proxy Operator list
    */
   vector<Sptr<Operator> > myOperators;


   /** Sip transceiver that receives the incoming sip message, and
    *  that is used to send outgoing sip messages. The sip thread
    *  uses the sip transceiver to read incoming sip messages.
    */
   Sptr < SipTransceiver > mySipStack;


   /** Ther sip thread reads the incoming sip messages from the 
    *  sip transceiver and posts them to the worker thread.
    */
   Sptr < SipThread > mySipThread;

private:
   /** Suppress copying
    */
   StatelessBasicProxy(const StatelessBasicProxy &);
   StatelessBasicProxy();

   /** Suppress copying
    */
   const StatelessBasicProxy & operator=(const StatelessBasicProxy &);
};
 
}

#endif // STATELESS_HEART_LESS_PROXY_HXX
