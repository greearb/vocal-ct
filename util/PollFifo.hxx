#if !defined(VOCAL_POLL_FIFO_HXX)
#define VOCAL_POLL_FIFO_HXX

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


static const char* const PollFifo_hxx_Version = 
    "$Id: PollFifo.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "FifoBase.h"
#include "Poll.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "VLog.hxx"


namespace Vocal {
    namespace Transport {
    	class Protocol;
    	class ProtocolException;
    }
}


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


class SystemException;
class SystemStatus;


using Vocal::Transport::Protocol;
using Vocal::Transport::Poll;


/** This is where is Fifo code meets the transport code.<br><br>
 *
 *  The mechanism to block is the Poll. Protocols can also register with 
 *  this Fifo. When an incoming message arrives, wait returns.
 *
 *  @see    FifoBase
 *  @see    Fifo
 */
template<class Msg>
class PollFifo : public FifoBase<Msg>
{
    public:

	/** Create an empty fifo.
	 */
	PollFifo();
				    

	/** Delete the fifo. Must be empty to delete.
	 */
	virtual     ~PollFifo();


    	/** Register the protocol for activity detection.
	 */
	void        registerProtocol(Protocol &);


    	/** Update the protocol's activity detection.
	 */
	void        updateProtocol(Protocol &);


    	/** Register the protocol for activity detection.
	 */
	void        unregisterProtocol(Protocol &);


    	/** Process the activity on the given protocols.
	 */
	void        processProtocols(int numberFdsActive)
		    	throw ( Vocal::Transport::ProtocolException );


        /** Poll accessor.
         */
        Poll &      poll();
        

        /** Const Poll accessor.
         */
        const Poll &    poll() const;
        
        
    private:

    	/** Wake the blocked thread.
	 */
    	virtual void	    wakeup() throw ( VException );


    	/** Wait until a message has arrived, or there is polled io
    	 *  activity. If there is polled io activity, the return
    	 *  value is the number of file descriptors active, otherwise
    	 *  0 is returned. The fifo must be empty to detect polled io
    	 *  activity.
    	 *  
    	 *  Note that the mutex is locked upon entry. It is the responsibility
    	 *  of the user to unlock the mutex if necessary, making sure
    	 *  to relock it before exit.
	 */
      	int         wait(Vocal::TimeAndDate::milliseconds_t relativeTimeout)
		        throw ( VException );


    	Poll	    myPoll;
};


#include "PollFifo.cc"


} // namespace Vocal


#endif // !defined(VOCAL_POLL_FIFO_HXX)
