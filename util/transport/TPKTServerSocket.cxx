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


static const char* const TPKTServerSocket_cxx_Version = 
    "$Id: TPKTServerSocket.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "global.h"
#include "TPKTServerSocket.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "IPAddress.hxx"
#include "TPKTClientSocket.hxx"
#include "VLog.hxx"
#include <cerrno>


using Vocal::Transport::TPKTServerSocket;
using Vocal::Transport::TPKTClientSocket;
using Vocal::Transport::StreamServerSocket;
using Vocal::Transport::AddressFamily;
using Vocal::Transport::IPAddress;
using Vocal::IO::file_descriptor_t;
using Vocal::Logging::VLog;



TPKTServerSocket::TPKTServerSocket(
    const char      	*   name,
    const char      	*   acceptedName,
    int     	    	    backlog
)
throw ( Vocal::SystemException )
    : 	StreamServerSocket(
    	    AddressFamily(AF_INET), 
	    backlog, 
	    (name ? name : "TPKTServer")),
	acceptedName_(acceptedName ? acceptedName : "")
{
}


TPKTServerSocket::TPKTServerSocket(
    const IPAddress 	&   localAddr,
    const char      	*   name,
    const char      	*   acceptedName,
    int     	    	    backlog
)
throw ( Vocal::SystemException )
    :	StreamServerSocket(
    	    localAddr, 
	    backlog, 
	    (name ? name : "TPKTServer")),
	acceptedName_(acceptedName ? acceptedName : "")
{
}


TPKTServerSocket::~TPKTServerSocket()
    throw ( Vocal::SystemException )
{
}


Sptr<TPKTClientSocket>
TPKTServerSocket::accept()
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    const string    fn("TPKTServerSocket::accept");
    VLog    	    log(fn);

    file_descriptor_t	newFd = INVALID;

    IPAddress	    	remoteAddr;
    socklen_t	    	length = remoteAddr.getAddressLength();
        
    if	(   (newFd = ::accept(fd_, 
    	    	    	remoteAddr.getAddress(), 
			&length)) == INVALID
	)
    {
    	int error = errno;
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without receiving the packet. If the socket is set 
	// blocking we can get an EINTR here, without receiving the packet.
	// We'll throw a SystemStatus.
	// 
	if ( error == EAGAIN || error == EINTR )
	{
            throw Vocal::SystemStatus(fn + " on accept(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    	}	

    	throw Vocal::SystemException(fn + ": " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }

    Sptr<TPKTClientSocket>	newClientSocket 
    	= new TPKTClientSocket(newFd, remoteAddr, acceptedName_.c_str());
	
    return ( newClientSocket );
}
