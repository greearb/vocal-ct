#if !defined(VOCAL_STREAM_CLIENT_SOCKET_HXX)
#define VOCAL_STREAM_CLIENT_SOCKET_HXX

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


static const char* const StreamClientSocket_hxx_Version = 
    "$Id: StreamClientSocket.hxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "Socket.hxx"
#include "Sptr.hxx"
#include "TransportAddress.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "ConnectionBrokenException.hxx"
#include <string>
#include <vector>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


/** Stream client socket is a connection-oriented abstraction for reliable 
 *  message transport.<br><br>
 *
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::Transport::AddressFamily
 *  @see    Vocal::IO::file_descriptor_t
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 *  @see    Vocal::Transport::ConnectionBrokenException
 */
class StreamClientSocket : public Vocal::Transport::Socket
{

    public:


    	/** Address family constructor. Create a socket whose local
	 *  address will be a default transport address corresponding 
	 *  to the given address family. This usually produces an 
	 *  "ANY" address that will be allocated by the operating 
	 *  system.
	 */
    	StreamClientSocket(
	    const AddressFamily     & 	addressFamily,
    	    const char      	    * 	name = 0
	)
    	    throw ( Vocal::SystemException );


    	/** TransportAddress constructor. Create a socket whose
	 *  local address is bound to the given local address.
	 */				
    	StreamClientSocket(
	    const TransportAddress  &   localAddr,
    	    const char      	    *   name = 0
	)
    	    throw ( Vocal::SystemException );


    	/** Connection constructor. Create a socket whose local address
	 *  is bound to the local address given, and is connected to
	 *  the remote address given.
	 */
    	StreamClientSocket(
    	    const TransportAddress  &   localAddr,
	    const TransportAddress  &   remoteAddr,
	    const char      	    *   name = 0
	)
    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Accepted connection constructor. Create a socket from an
	 *  already established socket, i.e. via accept. This socket
	 *  is connected to the remote address given. If the file
	 *  descriptor passed in is invalid, a new socket will
	 *  be created, bound to the an "ANY" address and connected
	 *  to the remote address.
	 */
    	StreamClientSocket(
	    IO::file_descriptor_t    	fd, 
    	    const TransportAddress  &   remoteAddr,
	    const char      	    *   name = 0
	)
    	    throw ( Vocal::SystemException, Vocal::SystemStatus );
    					

    	/** Virtual destructor
	 */
	virtual     	~StreamClientSocket();


    	/** To create a connection oriented circuit.
	 *  A nonblocking socket may issue an SystemStatus
	 *  exception with a status value of EINPROGRESS. It is
	 *  then possible to select for completion of the connection, 
	 *  i.e via Selector::Select, the socket for writing.
	 */
    	void	connect(const TransportAddress & remoteAddr)
		    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** To complete the connection when the nonblocking has 
	 *  been connect()-ed and the socket has return
	 *  an EINPROGRESS status.
	 */
    	void	completeConnection()
	    	    throw ( Vocal::SystemException );
				

    	/** Text versions of a connection oriented send. 
	 *  Use connect() first. This will transmit the 
	 *  string to the remote address specified in connect().
	 *  Returns the number of bytes sent. If 0 was returned, there
	 *  was an empty message to send, and nothing was sent. If a 
	 *  SystemStatus was thrown the socket was set non-blocking 
	 *  and it would have blocked (an EAGAIN status), or the 
	 *  socket was set blocking and it was interrupted by a signal
	 *  (an EINTR) signal. 
	 */
    	virtual int 	send(const string &)
		    	throw ( Vocal::SystemException,
				Vocal::SystemStatus,
				Vocal::Transport::ConnectionBrokenException );

    	/** Text versions of a connection oriented send. 
	 *  See send(const string &).
	  */
    	virtual int 	send(const char *)
		    	throw ( Vocal::SystemException,
				Vocal::SystemStatus,
				Vocal::Transport::ConnectionBrokenException );


    	/** Binary version of a connection oriented send. 
	 *  See send(const string &)
	 */
    	virtual int 	send(const vector<u_int8_t> &)
		    	throw ( Vocal::SystemException,
				Vocal::SystemStatus,
				Vocal::Transport::ConnectionBrokenException );


    	/** Binary version of a connection oriented send. 
	 *  See send(const string &)
	 */
    	virtual int 	send(const u_int8_t *, size_t)
		    	throw ( Vocal::SystemException,
				Vocal::SystemStatus,
				Vocal::Transport::ConnectionBrokenException );
				

    	/** Text version of a connection oriented receive. 
	 *  Use connect() first. This will receive the 
	 *  string from the remote address specified in connect().
	 *  Returns the number of received bytes. If a SystemStatus
	 *  was thrown either the socket was set non-blocking and it
	 *  would have blocked (status EAGAIN), or the socket was set blocking
	 *  and it was interrupted by a signal (status EINTR). 
	 */
    	virtual int 	receive(string &)
		    	throw ( Vocal::SystemException,
				Vocal::SystemStatus,
				Vocal::Transport::ConnectionBrokenException );


    	/** Text version of a connection oriented receive. 
	 *  See receive(string &).
	 */
    	virtual int 	receive(char *, size_t)
		    	throw ( Vocal::SystemException,
				Vocal::SystemStatus,
				Vocal::Transport::ConnectionBrokenException );


    	/** Binary version of a connection oriented receive. 
	 *  See receive(string &).
	 */
    	virtual int 	receive(vector<u_int8_t> &)
		    	throw ( Vocal::SystemException,
				Vocal::SystemStatus,
				Vocal::Transport::ConnectionBrokenException );
				

    	/** Binary version of a connection oriented receive. 
	 *  See receive(string &).
	 */
    	virtual int 	receive(u_int8_t *, size_t)
		    	throw ( Vocal::SystemException,
				Vocal::SystemStatus,
				Vocal::Transport::ConnectionBrokenException );


    	/** If connected, this will return a pointer to the 
	 *  remote address. Otherwise 0 will be returned.
	 */
    	Sptr<TransportAddress>	getRemoteAddress() const;


    	/** Write the socket to an ostream.
	 */
	virtual ostream &   	writeTo(ostream &) const;
		

    private:

    	int 	sendMessage(void *, int)
		    	throw ( Vocal::SystemException,
				Vocal::SystemStatus,
				Vocal::Transport::ConnectionBrokenException );

    	int 	recvMessage(void *, int)
		    	throw ( Vocal::SystemException,
				Vocal::SystemStatus,
				Vocal::Transport::ConnectionBrokenException );


    	Sptr<TransportAddress>	    remoteAddr_;
	bool	    	    	    localAddrUpdated_;
	bool	    	    	    connectionCompleted_;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_STREAM_CLIENT_SOCKET_HXX)
