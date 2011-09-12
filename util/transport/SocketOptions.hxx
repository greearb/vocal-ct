#if !defined(SOCKET_OPTIONS_DOT_H)
#define SOCKET_OPTIONS_DOT_H

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


static const char* const SocketOptions_hxx_Version = 
    "$Id: SocketOptions.hxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "SystemException.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class Socket;
class IPAddress;


/** This class decorates a Socket with the purpose of configuring the 
 *  socket.<br><br>
 *
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::SystemException
 *  @see    Vocal::Transport::IPAddress
 */
class SocketOptions
{
    public:


    	/** Create the SocketOption, decorating the given socket.
	 */    	    	
    	SocketOptions(Socket &);


    	/** destructor
	 */    	    	
    	~SocketOptions();


    	/** Enable/disable sending of keep-alive messages on 
	 *  connection-oriented sockets.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
    	void	    	keepAlive(bool on) 
		    	    throw ( Vocal::SystemException );


    	/** Query state of keepAlive.
	 */    	    	
	bool	    	keepAlive() const
		    	    throw ( Vocal::SystemException );


    	/** Enable BSD bug-to-bug compatibility. Linux only.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	bsdCompatible(bool on = true)
		    	    throw ( Vocal::SystemException );


    	/** Query state of bsdCompatibility.
	 */    	    	
	bool	    	bsdCompatible() const
		    	    throw ( Vocal::SystemException );


    	/** Indicates that the rules used in validating addresses supplied  
	 *  in a bind(2) call should allow reuse of local addresses.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	reuseAddress(bool on = true)
		    	    throw ( Vocal::SystemException );


    	/** Query state of reuseAddress.
	 */    	    	
	bool	    	reuseAddress() const		    
		    	    throw ( Vocal::SystemException );


    	/** When enabled, datagram sockets receive packets sent to a broadcast
    	 *  address and they are allowed to send packets to a broadcast 
	 *  address. This option has no effect on stream-oriented sockets.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	broadcast(bool on = true)
		    	    throw ( Vocal::SystemException );


    	/** Query state of broadcast.
	 */    	    	
	bool	    	broadcast() const
		    	    throw ( Vocal::SystemException );


    	/** When enabled, a close(2) or shutdown(2) will not return until all 
	 *  queued messages for the socket have been successfully sent or the 
	 *  linger timeout has been reached. Otherwise, the call returns 
	 *  immediately and the closing is done in the background.
	 *
	 *  See socket(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	linger(bool on, int seconds)
		    	    throw ( Vocal::SystemException );


    	/** Query state of linger. If enabled, the seconds will return
	 *  with the linger timeout.
	 */    	    	
	bool	    	linger(int & seconds) const
		    	    throw ( Vocal::SystemException );


    	/** Join a multicast group, as given in the IPAddress.
	 *
	 *  See ip(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	joinMulticastGroup(IPAddress & mcastAddr)
		    	    throw ( Vocal::SystemException );


    	/** Leave a multicast group, as given in the IPAddress.
	 *
	 *  See ip(7), and getsockopt/setsockopt(2).
	 */    	    	
	void	    	leaveMulticastGroup(IPAddress & mcastAddr)
		    	    throw ( Vocal::SystemException );

    private:

    	void	    	set(int level, int optname, bool)
		    	    throw ( Vocal::SystemException );


	bool	    	get(int level, int optname) const
		    	    throw ( Vocal::SystemException );
				    

    	Socket&	socket_;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(SOCKET_OPTIONS_DOT_H)
