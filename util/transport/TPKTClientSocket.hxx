#if !defined(VOCAL_TPKT_CLIENT_SOCKET_HXX)
#define VOCAL_TPKT_CLIENT_SOCKET_HXX

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


static const char* const TPKTClientSocket_hxx_Version = 
    "$Id: TPKTClientSocket.hxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "TCPClientSocket.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


/** TPKT is an extension of the TCPClientSocket that allows for
 *  packetizing the streamed information.<br><br>
 *
 *  Every packet send via the TPKT is prefixed with a header that first 
 *  indicates it is a TPKT, and that second indicates the length of the packet.
 *
 *  @see    Vocal::Transport::TCPClientSocket
 *  @see    Vocal::Transport::StreamClientSocket
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::Transport::IPAddress
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::IO::file_descriptor_t
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 */
class TPKTClientSocket : public Vocal::Transport::TCPClientSocket
{

    public:

    	/** Default constructor. Create a socket whose local
    	 *  address will be a default IP address. 
	 */
    	TPKTClientSocket(const char * name = 0)
    	    throw ( Vocal::SystemException );


    	/** TransportAddress constructor. Create a socket whose
    	 *  local address is bound to the given local address.
	 */				
    	TPKTClientSocket(
	    const IPAddress 	&   localAddr,
    	    const char      	*   name = 0
	)
    	    throw ( Vocal::SystemException );

    	/** Connection constructor. Create a socket whose local address
    	 *  is bound to the local address given, and is connected to
    	 *  the remote address given.
	 */
    	TPKTClientSocket(
	    const IPAddress 	&   localAddr,
	    const IPAddress 	&   remoteAddr,
	    const char      	*   name = 0
	)
    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Accepted connection constructor. Create a socket from an
    	 *  already established socket, i.e. via accept. This socket
    	 *  is connected to the remote address given. If the file
    	 *  descriptor passed in is invalid, a new socket will
    	 *  be created, bound to the an INADDR_ANY and connected
    	 *  to the remote address.
	 */
    	TPKTClientSocket(
	    IO::file_descriptor_t 	fd,
    	    const IPAddress 	    &   remoteAddr,
    	    const char      	    *   name = 0
	)
    	    throw ( Vocal::SystemException, Vocal::SystemStatus );


    	/** Virtual destructor
	 */        					
	virtual     	~TPKTClientSocket();


    	/** Text versions of a connection oriented send. 
    	 *  See StreamClientSocket::send(). This prefixes
    	 *  outgoing messages with the TPKT header.
	 */
    	virtual int 	    send(const string &)
		    	    throw ( Vocal::SystemException,
				    Vocal::SystemStatus,
				    Vocal::Transport::ConnectionBrokenException );


    	/** Text versions of a connection oriented send. 
    	 *  See StreamClientSocket::send(). This prefixes
    	 *  outgoing messages with the TPKT header.
	 */
    	virtual int 	    send(const char *)
		    	    throw ( Vocal::SystemException,
				    Vocal::SystemStatus,
				    Vocal::Transport::ConnectionBrokenException );


    	/** Binary version of a connection oriented send. 
    	 *  See send(const string &)
	 */
    	virtual int 	    send(const vector<u_int8_t> &)
		    	    throw ( Vocal::SystemException,
				    Vocal::SystemStatus,
				    Vocal::Transport::ConnectionBrokenException );


    	/** Binary version of a connection oriented send. 
    	 *  See send(const string &)
	 */
    	virtual int 	    send(const u_int8_t *, size_t)
		    	    throw ( Vocal::SystemException,
				    Vocal::SystemStatus,
				    Vocal::Transport::ConnectionBrokenException );
				

    	/** Will parse the TPKT header. If the entire TPKT header is not 
    	 *  fully received (if the underlying file descriptor is set
    	 *  nonblocking), a SystemStatus will be raised, with the status 
    	 *  set to EAGAIN. The state for the partially parsed header will 
    	 *  be saved unless clearTPKTHeader() is called. When the socket
    	 *  is again ready for reading, call this function again.
    	 *  Upon successful parsing of the header the returned size
    	 *  will be the size of the TPKT packet remaining. If the size
    	 *  is zero, either zero was sent, or the header wasn't correct.
	 */
    	u_int16_t   receiveTPKTHeader()
		    	    throw ( Vocal::SystemException,
				    Vocal::SystemStatus,
				    Vocal::Transport::ConnectionBrokenException );

    	/** Clear the stored TPKT header information.
	 */
    	void	    clearTPKTHeader();
		

    private:
    	void	    sendTPKTHeader(u_int16_t)
		    	    throw ( Vocal::SystemException,
				    Vocal::SystemStatus,
				    Vocal::Transport::ConnectionBrokenException );
		
    	static	const u_int8_t      TPKT_FLAG_[2];
	
    	    	u_int8_t    	    header_[4];
		int	    	    headerPosition_;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_TPKT_CLIENT_SOCKET_HXX)
