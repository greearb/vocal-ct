#if !defined(VOCAL_TCP_SERVER_PROTOCOL_HXX)
#define VOCAL_TCP_SERVER_PROTOCOL_HXX

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


static const char* const TCPServerProtocol_hxx_Version = 
    "$Id: TCPServerProtocol.hxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "Protocol.hxx"
#include "Sptr.hxx"
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


class TCPServerSocket;


/** Since there are no bytes flowing in a connection with a server socket,
 *  the meaning associated with incoming availability on the file descriptor
 *  is an incoming socket connection.<br><br> 
 *
 *  The TCPServerProtocol accepts this connection from the socket and 
 *  indicates to the user this via the onAcceptedConnection callback.
 *
 *  @see    Vocal::Transport::Protocol
 *  @see    Vocal::Transport::TCPServerSocket
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 *  @see    Sptr
 */
class TCPServerProtocol : public Vocal::Transport::Protocol
{

    public:


    	/** Create the protocol with an optional name.
	 */
    	TCPServerProtocol(const char * name = 0);


    	/** Virtual destructor.
	 */
    	virtual ~TCPServerProtocol();


    	/** Get the TCPServerSocket associated with this protocol.
	 */
    	virtual TCPServerSocket &     	    
	    	getTCPServerSocket() = 0;
		

    	/** Callback to notify the user of an accepted connection.
	 */
    	virtual void	    	    	    
	    	onAcceptedConnection(Sptr<TCPClientSocket>) = 0;


    	/** Override's Protocol's onIncomingAvailable. When a server socket
	 *  has incoming available, it needs to accept the connection and
	 *  then call onAcceptedConnection.
	 */
	virtual void	    	    	    
	    	onIncomingAvailable()
		throw ( Vocal::SystemException, Vocal::SystemStatus );
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_TCP_SERVER_PROTOCOL_HXX)
