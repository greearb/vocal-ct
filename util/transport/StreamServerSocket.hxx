#if !defined(STREAM_SERVER_SOCKET_DOT_H)
#define STREAM_SERVER_SOCKET_DOT_H

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


static const char* const StreamServerSocket_hxx_Version = 
    "$Id: StreamServerSocket.hxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "Socket.hxx"
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


class StreamClientSocket;


/** Stream server socket provides an address family agnostic way to create 
 *  a socket that will listen for incoming connections.<br><br>
 *
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::Transport::AddressFamily
 *  @see    Vocal::SystemException
 */
class StreamServerSocket : public Vocal::Transport::Socket
{

    protected:

    	/** Address family constructor. Create a server socket whose 
	 *  local address will be a default transport address 
	 *  corresponding to the given address family. This usually 
	 *  produces an "ANY" address that will be allocated by the 
	 *  operating system.
	 */
    	StreamServerSocket( 
	    const AddressFamily     & 	addressFamily,
    	    int     	    	    	backlog = 5,
    	    const char      	    * 	name = 0
	)
    	    throw ( Vocal::SystemException );


    	/** TransportAddress constructor. Create a socket whose
	 *  local address is bound to the given local address.
	 */
    	StreamServerSocket( 
	    const TransportAddress  &	localAddr,
	    int     	    	    	backlog = 5,
	    const char      	    *   name = 0
	)
    	    throw ( Vocal::SystemException );

    public:

    	/** Virtual destructor
	 */
	virtual ~StreamServerSocket();


    private:

    	void	listen(int backlog)
    	    	    throw ( Vocal::SystemException );
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(STREAM_SERVER_SOCKET_DOT_H)
