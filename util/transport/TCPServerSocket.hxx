#if !defined(VOCAL_TCP_SERVER_SOCKET_HXX)
#define VOCAL_TCP_SERVER_SOCKET_HXX

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


static const char* const TCPServerSocket_hxx_Version = 
    "$Id: TCPServerSocket.hxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "StreamServerSocket.hxx"
#include "Sptr.hxx"
#include "TCPClientSocket.hxx"
#include <string>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class IPAddress;


/** Provides a TCP/IP socket that will listen for incoming connections.<br><br>
 *
 *  @see    Vocal::Transport::TCPClientSocket
 *  @see    Vocal::Transport::StreamServerSocket
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::Transport::IPAddress
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 *  @see    Sptr
 */
class TCPServerSocket : public Vocal::Transport::StreamServerSocket
{

    public:


    	/** Create a server socket whose local address will be a 
	 *  default IP address, corresponding INADDR_ANY.
	 */
    	TCPServerSocket(
	    int     	    backlog = 5,
	    const char  *   name = 0,
	    const char  *   acceptedName = 0
	)
    	    throw ( Vocal::SystemException );


    	/** TransportAddress constructor. Create a socket whose
    	 *  local address is bound to the given local address.
	 */				
    	TCPServerSocket(
	    const IPAddress 	    &   localAddr,
	    int     	    	    	backlog = 5,
	    const char      	    *   name = 0,
	    const char      	    *   acceptedName = 0
	)
    	    throw ( Vocal::SystemException );

    	/** Virtual destructor.
	 */
	virtual     	~TCPServerSocket();


    	/** Accept an incoming connection, creating a TCPClientSocket.
    	 */
    	virtual Sptr<TCPClientSocket> 	accept()
    	  	    	    	    	throw ( Vocal::SystemException,
					    	Vocal::SystemStatus );

    private:
    	    	string	    	    	acceptedName_;					 
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_TCP_SERVER_SOCKET_HXX)
