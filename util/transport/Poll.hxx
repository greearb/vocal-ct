#if !defined(VOCAL_POLL_HXX)
#define VOCAL_POLL_HXX

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


static const char* const Poll_hxx_Version = 
    "$Id: Poll.hxx,v 1.2 2004/05/04 07:31:16 greear Exp $";


#include <map>
#include <vector>
#ifdef __APPLE__
#include "poll.h"
#else
#include <sys/poll.h>
#endif
#include "Sptr.hxx"
#include "Pipe.hxx"
#include "Writer.hxx"
#include "ProtocolException.hxx"
#include "ConnectionBrokenException.hxx"
#include "SystemException.hxx"


namespace Vocal {
    namespace IO {
        class FileDescriptor;
    }
}


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


using Vocal::IO::FileDescriptor;


class Protocol;


/* Needed for map of pollfds.
 */
struct pollfdLess
{
    bool operator()(const pollfd & left, const pollfd & right) const;
};


/** A Poll wraps the operating system's poll() call.<br><br>
 *  
 *  The Poll and Protocol classes together form a type of Subject/Observer
 *  (see Design Patterns, Gamma, et al. page 293) pattern. The Poll object
 *  is the subject whereas the Protocol is the Observer. The Protocol object
 *  register's itself with the Poll. The protocol is interested in being 
 *  notified when it's file descriptor is active. Thus the observer provides 
 *  and an aspect to the server to reduce messaging activity. As well, since 
 *  the observer provides an aspect, the push model is used.
 *
 *  @see    Vocal::Transport::Protocol
 *  @see    Vocal::SystemException
 *  @see    Vocal::Transport::ProtocolException
 */
class Poll : public Vocal::IO::Writer
{

    public:


    	/** Default constructor
	 */
    	Poll();


    	/** Virtual destructor
	 */
    	virtual     	    	    ~Poll();


        /** When registering a file descriptor with a poll, the event type 
         *  determines which types of events you are interested in polling 
         *  the descriptor for.
         */
        typedef u_int16_t   EventType;
        

        /** You want to receive on the descriptor.
         */
        static EventType    Incoming;


        /** You want to send on the descriptor. Most useful with a 
         *  nonblocking descriptor.
         */
        static EventType    Outgoing;


        /** You want to receive priority messages on the descriptor.
         */
        static EventType    Priority;
        

    	/** Register the file descriptor for activity detection. 
         *  You can logically or the EventTypes to listen to different
         *  types of events. 
	 */	
	void	    	    registerFD(const FileDescriptor &, 
                                       EventType flag = Incoming,
                                       Protocol * = 0);


    	/** Update the flags for the file descriptor.
	 */	
    	void	    	    updateFD(const FileDescriptor &, 
                                     EventType flag = Incoming,
                                     Protocol * = 0);


    	/** No longer detect activity for protocol's file descriptor.
	 */	
	void	    	    unregisterFD(const FileDescriptor &);
        

    	/** Register the protocol's file descriptor for
    	 *  activity detection. 
	 */	
	void	    	    registerProtocol(Protocol &);


    	/** Update the flags for protocol's file descriptor for
    	 *  activity detection. 
	 */	
    	void	    	    updateProtocol(Protocol &);


    	/** No longer detect activity for protocol's file descriptor.
	 */	
	void	    	    unregisterProtocol(Protocol &);


    	/** A blocking call waiting for activity on the
    	 *  file descriptors of the registered sockets and protocols.
         *  This will block indefinately if the timeout is -1, or 
         *  for the number of milliseconds specified.
         *
    	 *  If a signal interrupts the poll, 0 is returned,
    	 *  otherwise the number of active file descriptors is
    	 *  returned.
	 */
    	int 	    	    poll(int timeout = -1)
		    	    	throw ( Vocal::SystemException );


        /** See if the given file descritor is active.
         */
        EventType           fdActive(const FileDescriptor &) const;
        
        
    	/** Process the activity on the given protocols.
	 */
	void 	    	    processProtocols(int numberFdsActive)
    	    	    	    	throw ( Vocal::Transport::ProtocolException );


    	/** Wake poll(). interrupt() should be called from a thread
    	 *  different from the thread that calls poll().
	 */		
	void	    	    interrupt()
		    	    	throw ( Vocal::SystemException );


	/** Returns the number of outstanding interrupts pending on the 
	 *  poll, if any.
	 */
	u_int8_t    	    interruptCount() const;
		

    	/** Write the poll to an ostream.
	 */
	virtual ostream &           writeTo(ostream &) const;
					

    private:

    	void	    	    addInterruptorToFds();

	void	    	    addProtocolsToFds();

    	bool	    	    processInterruptor(pollfd &);

    	bool	    	    processPriority(pollfd &, Protocol *);

    	bool	    	    processIncoming(pollfd &, Protocol *);

    	bool	    	    processOutgoing(pollfd &, Protocol *);

    	bool	    	    processHangup(
		    	    	pollfd &, 
				Protocol *,
				ConnectionBrokenException * = 0);

    	bool	    	    processError(
		    	    	pollfd &, 
				Protocol *,
				SystemException * = 0);

     	typedef     map< pollfd, 
 		    	 Protocol *, 
 		    	 pollfdLess> 	ProtocolMap;

	ProtocolMap 	    myProtocols;
	bool	    	    myProtocolsChanged;

    	vector<pollfd>	    myFds;

    	IO::Pipe	    myInterruptor;
	u_int8_t   	    myInterruptCount;

    	// Suppress copying
	//
    	Poll(const Poll &);
	Poll & operator=(const Poll &);
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_POLL_HXX)
