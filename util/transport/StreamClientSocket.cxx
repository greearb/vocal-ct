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


static const char* const StreamClientSocket_cxx_Version = 
    "$Id: StreamClientSocket.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "global.h"
#include "TransportCommon.hxx"
#include "StreamClientSocket.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "ConnectionBrokenException.hxx"
#include "VLog.hxx"
#include <cerrno>


using Vocal::Transport::StreamClientSocket;
using Vocal::Transport::Socket;
using Vocal::Transport::TransportAddress;
using Vocal::Transport::AddressFamily;
using Vocal::IO::file_descriptor_t;
using Vocal::Logging::VLog;
using Vocal::SystemException;
using Vocal::SystemStatus;


StreamClientSocket::StreamClientSocket(
    const AddressFamily     & 	addressFamily,
    const char	    	    *	name
)
throw ( Vocal::SystemException )
    :   Socket( addressFamily, 
    	    	SocketType(SOCK_STREAM), 
		(name ? name : "StreamClient")),
    	remoteAddr_(0),
	localAddrUpdated_(false),
	connectionCompleted_(false)
{
}


StreamClientSocket::StreamClientSocket(
    const TransportAddress  & 	localAddr,
    const char      	    *	name
)
throw ( Vocal::SystemException )
    :	Socket( localAddr, 
    	    	SocketType(SOCK_STREAM), 
		(name ? name : "StreamClient")),
    	remoteAddr_(0),
	localAddrUpdated_(false)
{
}


StreamClientSocket::StreamClientSocket(
    const TransportAddress  & 	localAddr,
    const TransportAddress  & 	remoteAddr,
    const char      	    *	name
)
throw ( Vocal::SystemException, Vocal::SystemStatus )
    :	Socket( localAddr, 
    	    	SocketType(SOCK_STREAM), 
		(name ? name : "StreamClient")),
    	remoteAddr_(0),
	localAddrUpdated_(false)
{
    // Connect to the remote address, Don't catch the exception or status,
    // let it pass.
    //
    connect(remoteAddr);
}
    					

StreamClientSocket::StreamClientSocket(
    file_descriptor_t 	    	fd, 
    const TransportAddress  &   remoteAddr,
    const char      	    * 	name
)
throw ( Vocal::SystemException, Vocal::SystemStatus )
    :	Socket( remoteAddr.getAddressFamily(), 
    	    	SocketType(SOCK_STREAM), 
		(name ? name : "StreamClient"),
		fd),
	remoteAddr_(remoteAddr.clone()),
	localAddrUpdated_(false)
{
    // If they gave us an invalid fd descriptor, the Socket constructor
    // created one and bound it to ANY. Thus we haven't been connected,
    // so let's try to connect.
    //
    if ( fd == INVALID )
    {
    	connect(remoteAddr);
    }
}


StreamClientSocket::~StreamClientSocket()
{
}


void	
StreamClientSocket::connect(
    const TransportAddress  & 	remoteAddr
)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    const string    fn("StreamClientSocket::connect");
    VLog    	    log(fn);

    if ( remoteAddr_ == 0 )
    {    
        remoteAddr_ = remoteAddr.clone();
    }

    if	(    ::connect(fd_, 
    	    	     remoteAddr_->getAddress(), 
		     remoteAddr_->getAddressLength()) < SUCCESS
	)
    {
    	int error = errno;

    	// If the socket is nonblocking and the connection will take
	// a while to complete, we'll get EINPROGRESS back. This is
	// ok, just send a status with EINPROGRESS back. The user will
	// have to use completeConnection() later on.
	//	
    	if ( error == EINPROGRESS )
	{
	    throw Vocal::SystemStatus(fn + " on connect(): " + strerror(error),
	    	    	    	__FILE__, __LINE__, error);
	}

    	// Otherwise we have a problem. Signal it with an exception.
	//	
    	throw Vocal::SystemException(fn + " on connect(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }
    else
    {
    	connectionCompleted_ = true;
    }

    VDEBUG(log) << fn << ": fd = " << fd_ 
    	    	<< " connected to remote address = "
		<< *remoteAddr_ << VDEBUG_END(log);
}


void	
StreamClientSocket::completeConnection()
throw ( Vocal::SystemException )
{
    if ( !remoteAddr_ )
    {
    	return;
    }
    
    const string    fn("StreamClientSocket::completeConnection");
    VLog    	    log(fn);

    int     	connectCode = 0;
    socklen_t	connectCodeSize = sizeof(connectCode);

    if ( connectionCompleted_ )
    {
    	return;
    }

    if  (   getsockopt( fd_, SOL_SOCKET, SO_ERROR, 
	    	    	#if defined(__sun) || defined(WIN32)
			(char *)
			#endif // defined(__sun)
    	    	        &connectCode, 
		        &connectCodeSize) < SUCCESS
	||  connectCode < SUCCESS 
	)
    {
    	int error = ( connectCode < SUCCESS ? connectCode : errno );
	
    	throw Vocal::SystemException(fn + " on connect(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }    	

    VDEBUG(log) << fn << ": fd = " << fd_ 
    	    	<< " connection completed to remote address = "
		<< *remoteAddr_ << VDEBUG_END(log);
}
				

int 	
StreamClientSocket::send(const string & message)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    void * msg = reinterpret_cast<void *>(const_cast<char *>(message.c_str()));

    return ( sendMessage(msg, message.size()) );
}


int 	
StreamClientSocket::send(const char * message)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    int messageSize = ( message ? strlen(message) : 0 );
    
    void * msg = reinterpret_cast<void *>(const_cast<char *>(message));
    
    return ( sendMessage(msg, messageSize) );
}


int 	
StreamClientSocket::send(const vector<u_int8_t> & message)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
    	Vocal::Transport::ConnectionBrokenException )
{
    void * msg = reinterpret_cast<void *>(const_cast<u_int8_t *>(&message[0]));

    return ( sendMessage(msg, message.size()) );
}


int 	
StreamClientSocket::send(const u_int8_t * message, size_t messageSize)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    void * msg = reinterpret_cast<void *>(const_cast<u_int8_t *>(message));

    return ( sendMessage(msg, messageSize) );
}


int 	
StreamClientSocket::receive(string & message)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    void * msg = reinterpret_cast<void *>(const_cast<char *>(message.data()));

    int bytesReceived = recvMessage(msg, message.capacity());

    message.resize(bytesReceived);
        
    return ( bytesReceived );
}


int 	
StreamClientSocket::receive(char * message, size_t messageCapacity)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    if ( message == 0 || messageCapacity == 0 )
    {
    	return ( 0 );
    }
    
    void * msg = reinterpret_cast<void *>(message);

    int bytesReceived = recvMessage(msg, (messageCapacity > 1 ? messageCapacity-1 : 1 ));

    if ( messageCapacity > 1 )
    {    
    	message[bytesReceived] = '\0';
    }

    return ( bytesReceived );
}


int 	
StreamClientSocket::receive(vector<u_int8_t> & message)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    message.clear();
    
    void * msg = reinterpret_cast<void *>(&message[0]);

    int bytesReceived = recvMessage(msg, message.size());

    message.resize(bytesReceived);

    return ( bytesReceived );
}
				

int 	
StreamClientSocket::receive(u_int8_t * message, size_t messageCapacity)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    if ( message == 0 || messageCapacity == 0 )
    {
    	return ( 0 );
    }
    
    void * msg = reinterpret_cast<void *>(message);

    int bytesReceived = recvMessage(msg, messageCapacity);
    
    return ( bytesReceived );
}


Sptr<TransportAddress>
StreamClientSocket::getRemoteAddress() const
{
    return ( remoteAddr_ );
}
		

ostream &           	
StreamClientSocket::writeTo(ostream & out) const
{
    Socket::writeTo(out);
    
    if ( remoteAddr_ != 0 )
    {
    	out << ", remote = " << *remoteAddr_;
    }

    return ( out );
}

int 	
StreamClientSocket::sendMessage(
    void    	    	    *	message, 
    int     	    	    	messageLength
)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    const string    fn("StreamClientSocket::send");
    VLog    	    log(fn);

    sockbuf_t   *   msg = static_cast<sockbuf_t *>(message);

    int bytesSent = ::send( fd_, 
    	    	    	    msg, 
			    messageLength, 
			    MSG_NOSIGNAL);
    
    if ( bytesSent < SUCCESS )
    {
    	int error = errno;
	
	// If the other side disappeared we could get an EPIPE here.
	// Issue the ConnectionBroken exception since this is a situation 
	// that needs to be cleaned up by the user.
	//
	if ( error == EPIPE )
	{
	    throw Vocal::Transport::ConnectionBrokenException(
	    	    	    fn + " on send(): " + strerror(error), 
	    	    	    __FILE__, __LINE__, error);
	}
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without sending the packet. If the socket is set 
	// blocking we can get an EINTR here, without sending the packet.
	// We'll throw a SystemStatus.
	// 
	if ( error == EAGAIN || error == EINTR )
	{
            throw Vocal::SystemStatus(fn + " on send(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    	}	

    	throw Vocal::SystemException(fn + " on send(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }

    totalBytesSent_ += bytesSent;

    VDEBUG(log) << fn << ": send on " << *this
		<< ", bytes sent: " << bytesSent
		<< ", of total: " << messageLength
		<< VDEBUG_END(log);

    return ( bytesSent );
}
				

int 	
StreamClientSocket::recvMessage(
    void    	    	    *	message, 
    int     	    	    	messageCapacity
)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    const string    fn("StreamClientSocket::recv");
    VLog    	    log(fn);

    sockbuf_t   *   msg = static_cast<sockbuf_t *>(message);

    int bytesReceived = ::recvfrom( fd_, 
    	    	    	    	    msg, 
				    messageCapacity, 
				    MSG_NOSIGNAL,
	    	    	    	    0, 
				    0);
				
    if ( bytesReceived <= 0 )
    {
    	int error = errno;

    	// If the other side has initiated a disconnect, we will get 0 bytes
	// received. If the other side disappeared we could get an EPIPE here.
	// Issue the ConnectionBroken exception since this is a situation 
	// that needs to be cleaned up by the user.
	//
	if ( bytesReceived == 0 || error == EPIPE )
	{
	    error = EPIPE;
	    
	    throw Vocal::Transport::ConnectionBrokenException(
	    	    	    fn + " on recv(): " + strerror(error), 
	    	    	    __FILE__, __LINE__, error);
	}
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without receiving the packet. If the socket is set 
	// blocking we can get an EINTR here, without receiving the packet.
	// We'll throw a SystemStatus.
	// 
	if ( error == EAGAIN || error == EINTR )
	{
            throw Vocal::SystemStatus(fn + " on recv(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    	}	

    	throw Vocal::SystemException(fn + " on recv(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }

    if ( !localAddrUpdated_ )
    {
    	localAddrUpdated_ = true;
    	localAddr_->updateAddress(*this);
    }
        
    totalBytesReceived_ += bytesReceived;

    VDEBUG(log) << fn << ": received on " << *this
		<< ", bytes received: " << bytesReceived
		<< ", of capacity: " << messageCapacity
		<< VDEBUG_END(log);

    return ( bytesReceived );
};
