#ifndef UaWorkerThread_hxx
#define UaWorkerThread_hxx

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


static const char* const UaWorkerThread_hxx_Version = 
    "$Id: UaWorkerThread.hxx,v 1.1 2004/05/01 04:15:25 greear Exp $";


#include "Fifo.h"
#include "Sptr.hxx"
#include "ThreadIf.hxx"
#include "SipEvent.hxx"
#include "UaCallControl.hxx"

using Vocal::Fifo;
using Vocal::SipProxyEvent;


/** UaWorkerThread is a ThreadIf. 
 *  The thread waits on a fifo for SIP, device, user interaction events
 *  and dispatches them to the call-controller for processing.
 */
class UaWorkerThread: public ThreadIf
{
    public:
        /** Create the worker thread given the incoming sip event fifo and
         *  the builder. The incoming messages from the fifo will be 
         *  distributed to the builder.
         */
        UaWorkerThread( const Sptr < Fifo < Sptr < SipProxyEvent > > > inputFifo) 
              : ThreadIf(0, VTHREAD_PRIORITY_DEFAULT, VTHREAD_STACK_SIZE_DEFAULT),
                myFifo(inputFifo)
        { 
            assert( myFifo != 0 );
            allDone = 0;
        };


        /** Virtual destructor
         */
        virtual ~UaWorkerThread() { assert((allDone == 0) || (allDone == 2)); };


        /** Indicates to the thread that it needs to cease processing.
         */
        virtual void shutdown();

    protected:
        /** The method that does the work of distributing the sip events
         *  to the builder.
         */
        virtual void thread();


    private:
        /** Incoming sip events will come through this fifo. They will then
         *  be distributed to the builder.
         */
        Sptr < Fifo < Sptr < SipProxyEvent > > > myFifo;


        /** Suppress copying
         */
        UaWorkerThread(const UaWorkerThread &);
        
        
        /** Suppress copying
         */
        const UaWorkerThread & operator=(const UaWorkerThread &);

        int allDone; //Track state, for debugging purposes.
};

#endif
