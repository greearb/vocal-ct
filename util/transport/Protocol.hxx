#if !defined(VOCAL_PROTOCOL_HXX)
#define VOCAL_PROTOCOL_HXX

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


#include "NonCopyable.hxx"
#include "Writer.hxx"
#include "ProtocolException.hxx"
#include "SystemException.hxx"
#include <string>
#include <RCObject.hxx>



// Creating a forward reference here due to doc++ bug.
//
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


class ConnectionBrokenException;
class Poll;


/** A Protocol is the object that gives meaning to the bytes
 *  available from a FileDescriptor.<br><br>
 *  
 *  The Poll and Protocol classes together form a type of Subject/Observer
 *  (see Design Patterns, Gamma, et al. page 293) pattern. The Poll object
 *  is the subject whereas the Protocol is the Observer. The Protocol object
 *  register's itself with the Poll. The protocol is interested in being 
 *  notified when it's file descriptor is active. Thus the observer provides 
 *  and an aspect to the server to reduce messaging activity. As well, since 
 *  the observer provides an aspect, the push model is used.
 *
 *  @see    Vocal::Transport::Poll
 *  @see    Vocal::Signaling::PollFifo
 *  @see    Vocal::IO::FileDescriptor
 *  @see    Vocal::ConnectionBrokenException
 */
   class Protocol : public Vocal::IO::Writer, public Vocal::NonCopyable
{
    public:

    	/** Create with an optional name.
	 */
    	Protocol(const char * name = 0);


    	/** Create with an optional name.
	 */
    	Protocol(Poll & poll, const char * name = 0);


    	/** Virtual destructor
	 */
        virtual ~Protocol() throw (Vocal::SystemException);
	

    	/** FileDescriptor accessor.
	 */
	virtual const IO::FileDescriptor &  getFileDescriptor() const = 0;
	

    	/** Callback that notifies the user of information available on the
	 *  associated FileDescriptor. The default behavior is to do 
	 *  nothing, thus this should be overridden by the user.
	 */
	virtual void	onIncomingAvailable();


    	/** Callback that notifies the user of the ability to send information 
	 *  on the associated FileDescriptor. The default behavior is to do 
	 *  nothing, thus this should be overridden by the user.
	 */
	virtual void	onOutgoingAvailable();


    	/** Callback that notifies the user of priority information available 
	 *  on the associated FileDescriptor. The default behavior is to do 
	 *  nothing, thus this should be overridden by the user.
	 */
	virtual void	onPriority();


    	/** Callback that notifies the user that the associated FileDescriptor
	 *  has disconnected. If a ConnectionBrokenException had been caught,
	 *  it is passed along. The default behavior is to do throw
	 *  a HANGUP_EXCEPTION.
	 */
	virtual void	onDisconnect(ConnectionBrokenException * = 0)
    	    	    	    throw ( Vocal::Transport::ProtocolException );


    	/** Callback that notifies the user that an error has been detected 
	 *  on the associated FileDescriptor. If a SystemException had 
	 *  been caught, it is passed along. The default behavior is to 
	 *  do throw an ERROR_EXCEPTION.
	 */
	virtual void	onError(SystemException * = 0)
    	    	    	    throw ( Vocal::Transport::ProtocolException );


    	/** If the poll receives an unknown exception, it calls this method,
	 *  which by default rethrows the exception. 
	 */
    	virtual void	handleUnknownException();
					    

    	/** Allow the Protocol to be notified of incoming data.
	 */
    	void	    	awaitIncoming(bool);


    	/** Query the Protocol state to see if it is to be notified of 
	 *  incoming data.
	 */
	bool	    	awaitIncoming() const;


    	/** Allow the Protocol to be notified of outgoing availability.
	 */
    	void	    	awaitOutgoing(bool);


    	/** Query the Protocol state to see if it is to be notified of 
	 *  outgoing availability.
	 */
    	bool	    	awaitOutgoing() const;


    	/** Allow the Protocol to be notified of priority messages.
	 */
    	void	    	awaitPriority(bool);


    	/** Query the Protocol state to see if it is to be notified of 
	 *  priority messages.
	 */
    	bool	    	awaitPriority() const;


        /**
         */
        const string &  name() const;
        
        
    	/** Write the protocol to an ostream.
	 */
	virtual ostream &   writeTo(ostream &) const;


    	/** Equality operator.
	 */
    	bool	    	operator==(const Protocol &);


    	/** Less than relational operator.
	 */
    	bool	    	operator<(const Protocol &);
		

    private:
		bool	    	    	    myIncoming;
    	    	bool	    	    	    myOutgoing;
		bool	    	    	    myPriority;
		string      	    	    myName;
                Poll                    *   myPoll;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_PROTOCOL_HXX)
