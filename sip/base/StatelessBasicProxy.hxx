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
    "$Id: StatelessBasicProxy.hxx,v 1.1 2004/05/01 04:15:25 greear Exp $";


#include "Data.hxx"
#include "Fifo.h"
#include "Operator.hxx"
#include "SipTransceiver.hxx"
#include "StatelessWorkerThread.hxx"
#include "cpLog.h"

class HeartbeatTxThread;
class HeartbeatRxThread;
class HouseKeepingThread;

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
class StatelessBasicProxy
{
    public:


        /** Explained in Uasge of this Class section.
         * local_ip may be "", in which case we'll use the default IP and/or bind
         *  to all interfaces.
         * hashTableSize is initial bucket count for underlying hash table(s).
         * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
	 */
        StatelessBasicProxy( int hashTableSize,
                             const string&            local_ip,
                             const string&            local_dev_to_bind_to,
                             unsigned short          defaultSipPort = 5060,
                             Data                    applName = "unknown", 
                             bool                    filteron = true, 
                             bool                    nat = false,
                             SipAppContext           cContext = APP_CONTEXT_GENERIC);


        /** Virtual destructor
         */
        virtual ~StatelessBasicProxy();


        /** Runs the underlying sip thread and worker thread.
         */
        virtual void run();


        /** Shutdown the underlying sip thread and worker thread.
         */
        virtual void shutdown();

        void addOperator(const Sptr < Operator > &   op)
        {
            cpLog(LOG_DEBUG, "Operator %s" , op->name());
            myOperators.push_back(op);
        }


        /** Joins the underlying sip thread and worker thread.
         */
        virtual void join();

        /** Calls the heartbeat tx run method.
         *  <B>Deprecated</B> call run() instead that also
         *  runs the heartbeat threads
         */
        virtual void runHeartbeatThread();

        ///
        void process(const Sptr < SipProxyEvent > event) const;

        /** Shuts down the three heartbeat threads.
         */
        virtual void shutdownHeartbeatThread();


        /** Calls the three heartbeat thread join methods
         */
        virtual void joinHeartbeatThread();

    protected:
        /** Proxy Operator list
         */
        vector<Sptr<Operator> > myOperators;

        /** Shared call processing queue between the sip thread and the
         *  worker thread. The sip thread writes to the queue and the
         *  sip thread reads from the queue.
         */
        Sptr < Fifo < Sptr < SipProxyEvent > > > myCallProcessingQueue;


        /** Sip transceiver that receives the incoming sip message, and
         *  that is used to send outgoing sip messages. The sip thread
         *  uses the sip transceiver to read incoming sip messages.
         */
        Sptr < SipTransceiver > mySipStack;


        /** The worker thread reads incoming sip events and processes them,
         *  via the supplied builder.
         */
        Sptr < StatelessWorkerThread > myWorkerThread;


        /** Ther sip thread reads the incoming sip messages from the 
         *  sip transceiver and posts them to the worker thread.
         */
        Sptr < SipThread > mySipThread;

        /** Pointer to heartbeat tx thread object
         */ 
        Sptr < HeartbeatTxThread >  myHeartbeatTxThread;
        

        /** Pointer to heartbeat rx thread object
         */ 
        Sptr < HeartbeatRxThread >  myHeartbeatRxThread;
        

        /** Pointer to heartbeat housekeeping thread object
         */
        Sptr < HouseKeepingThread > myHouseKeepingThread;
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
