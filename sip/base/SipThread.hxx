#ifndef SIP_THREAD_HXX
#define SIP_THREAD_HXX

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


static const char* const SipThread_hxx_Version = 
    "$Id: SipThread.hxx,v 1.1 2004/05/01 04:15:25 greear Exp $";


#include "Fifo.h"
#include "Sptr.hxx"
#include "ThreadIf.hxx"

//#include "SipTransceiver.hxx"
//#include "SipProxyEvent.hxx"
//#include "SipMsg.hxx"

namespace Vocal
{

class SipTransceiver;
class SipProxyEvent;
class SipMsg;

/** Object SipThread
<pre>
<br> Usage of this Class </br>

    SipThread is derived from ThreadIf.
    The class acts as a message dispatcher between SIP stack
    and application. It dispatches messages received from the SIP stack to
    the application after translating into a suitable event.
    SipThread blocks on sipstack receive.  On receiving a SipMsg it creates a
    SipEvent. The SipEvent then gets posted to fifo which application gets
    and processes.
</pre>
 */
class SipThread: public ThreadIf
{
    public:

        /** Create the sip thread given the sip transceiver and the fifo.
         *  The incoming messages from the sip transceiver will be queued on 
         *  the given fifo.
         *  For Marshal and RS proxies, no CallLegHistory is required
         */
        SipThread( const Sptr < SipTransceiver > sipStack,
		   const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo ,
                   bool callLegHistory=true);


        /** Virtual destructor.
         */
        virtual ~SipThread();


        /** Returns true if a loop is detected in the via list.
         */
        bool discardMessage(Sptr < SipMsg > & sipMsg) const;


    protected:


        /** Overload's ThreadIf::thread() and handles the bulk of the 
         *  processing. The incoming messages from the sip transceiver 
         *  will be queued on the given fifo.
         */
        virtual void thread();


        /** Source of incoming sip events.
         */
        Sptr < SipTransceiver > mySipStack;


        /** Sink for incoming sip events.
         */
        Sptr < Fifo < Sptr < SipProxyEvent > > > myOutputFifo;


    private:

    
        /** Suppress copying
         */
        SipThread(const SipThread &);
        

        /** Suppress copying
         */
        const SipThread * operator=(const SipThread &);

        bool myCallLegHistory;
};
 
}

#endif // SIP_THREAD_HXX
