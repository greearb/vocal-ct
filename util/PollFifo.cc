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


static const char* const PollFifo_cc_Version = 
    "$Id: PollFifo.cc,v 1.1 2004/05/01 04:15:33 greear Exp $";


using Vocal::Logging::VLog;


template <class Msg>
PollFifo<Msg>::PollFifo()
{
}


template <class Msg>
PollFifo<Msg>::~PollFifo()
{
}


template <class Msg>
void	    	    
PollFifo<Msg>::registerProtocol(Protocol & protocol)
{
    myPoll.registerProtocol(protocol);
}


template <class Msg>
void	    	    
PollFifo<Msg>::updateProtocol(Protocol & protocol)
{
    myPoll.updateProtocol(protocol);
}


template <class Msg>
void	    	    
PollFifo<Msg>::unregisterProtocol(Protocol & protocol)
{
    myPoll.unregisterProtocol(protocol);
}


template <class Msg>
void 	    	    
PollFifo<Msg>::processProtocols(int numberFdsActive)
throw ( Vocal::Transport::ProtocolException )
{
    myPoll.processProtocols(numberFdsActive);
}


template <class Msg>
Poll &      
PollFifo<Msg>::poll()
{
    return ( myPoll );
}
        

template <class Msg>
const Poll &    
PollFifo<Msg>::poll() const
{
    return ( myPoll );
}


template <class Msg>
void	    
PollFifo<Msg>::wakeup() 
throw ( VException )
{
    myPoll.interrupt();    
}


template <class Msg>
int	    	    
PollFifo<Msg>::wait(Vocal::TimeAndDate::milliseconds_t relativeTimeout)
throw ( VException )
{
    const string    fn("PollFifo::wait");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": poll, timeout = " << relativeTimeout
	    	<< VDEBUG_END(log);

    // The mutex is locked coming into this method. Unlock it while we
    // block so others can use the Fifo.
    //
    myMutex.unlock();
    
    // Block, waiting for io activity. This may throw a transport
    // exception. If so, let it pass through.
    //
    // If poll receives a UNIX signal, i.e. SIGTERM, it will return 0.
    //
    int numberFdsActive = myPoll.poll(relativeTimeout);

    // Lock the Fifo back up.
    //
    myMutex.lock();

    VDEBUG(log) << fn << ": poll returned. fds active = " << numberFdsActive
	    	<< VDEBUG_END(log);

    // Return the number of file descriptors active.
    //
    return ( numberFdsActive );
}
