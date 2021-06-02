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


#include <string.h>

#include "global.h"
#include "TransportCommon.hxx"
#include "DatagramSocket.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "VLog.hxx"
#include <cerrno>
#include <sys/types.h>


using Vocal::Transport::DatagramSocket;
using Vocal::Transport::Socket;
using Vocal::Transport::TransportAddress;
using Vocal::Transport::AddressFamily;
using Vocal::Transport::SocketType;
using Vocal::Logging::VLog;


DatagramSocket::DatagramSocket(uint16 tos, uint32 priority,
                               const AddressFamily& addressFamily,
                               const char* name)
//throw ( Vocal::SystemException )
      : 	Socket( tos, priority, addressFamily, 
    	    	SocketType(SOCK_DGRAM), 
		(name ? name : "Datagram")),
    	remoteAddr_(0),
	localAddrUpdated_(false)
{
}

DatagramSocket::DatagramSocket(uint16 tos, uint32 priority,
                               const TransportAddress&   localAddr,
                               const char*   name)
//   throw ( Vocal::SystemException )
      : Socket(tos, priority, localAddr, 
               SocketType(SOCK_DGRAM), 
               (name ? name : "Datagram")),
    	remoteAddr_(0),
	localAddrUpdated_(false)
{
}


DatagramSocket::DatagramSocket(uint16 tos, uint32 priority,
                               const TransportAddress& localAddr,
                               const TransportAddress& remoteAddr,
                               const char* name)
//   throw ( Vocal::SystemException )
      : Socket(tos, priority, localAddr, 
               SocketType(SOCK_DGRAM), 
               (name ? name : "Datagram")),
    	remoteAddr_(0),
	localAddrUpdated_(false)
{
    connect(remoteAddr);
}


void	
DatagramSocket::connect(const TransportAddress& remoteAddr)
   //throw ( Vocal::SystemException )
{
   const string    fn("DatagramSocket::connect");
   VLog    	    log(fn);
   
   if ( remoteAddr_ == 0 ) {
      remoteAddr_ = remoteAddr.clone();
   }
    
   if (::connect(fd_, remoteAddr_->getAddress(), remoteAddr_->getAddressLength()) < SUCCESS) {
      throw Vocal::SystemException(fn + " on connect(): " + strerror(errno), 
                                   __FILE__, __LINE__, errno);
   }

   VDEBUG(log) << fn << ": fd = " << fd_ 
               << " connected to remote address = "
               << *remoteAddr_ << VDEBUG_END(log);
}

void	
DatagramSocket::disconnect()
//throw ( Vocal::SystemException )
{
    const string    fn("DatagramSocket::disconnect");
    VLog    	    log(fn);

    if ( !remoteAddr_ )
    {
    	return;
    }
    
    sockaddr	sa;
    sa.sa_family = AF_UNSPEC;
    
    if ( ::connect(fd_, &sa, sizeof(sa)) )
    {
    	throw Vocal::SystemException(fn + " on disconnect(): " + strerror(errno), 
	    	    	    	__FILE__, __LINE__, errno);
    }

    VDEBUG(log) << fn << ": fd = " << fd_ 
    	    	<< " disconnected from remote address = "
		<< *remoteAddr_ << VDEBUG_END(log);

    remoteAddr_ = 0;
}
			    	

int 	
DatagramSocket::send(const string & message)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( !remoteAddr_ || message.size() == 0 )
    {
    	return ( 0 );
    }

    void * msg = reinterpret_cast<void *>(const_cast<char *>(message.c_str()));
    
    return ( sendMessage(msg, message.size(), 0) );
}


int 	
DatagramSocket::send(const char * message)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    size_t  messageLength = ( message ? strlen(message) : 0 );
    
    if ( !remoteAddr_ || messageLength == 0 )
    {
    	return ( 0 );
    }

    void * msg = reinterpret_cast<void *>(const_cast<char *>(message));
    
    return ( sendMessage(msg, messageLength, 0) );
}


int 	
DatagramSocket::send(const vector<u_int8_t> & message)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( !remoteAddr_ || message.size() == 0 )
    {
    	return ( 0 );
    }

    void * msg = reinterpret_cast<void *>(const_cast<u_int8_t *>(&message[0]));

    return ( sendMessage(msg, message.size(), 0) );
}


int 	
DatagramSocket::send(const u_int8_t * message, size_t messageLength)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( !remoteAddr_ || message == 0 || messageLength == 0 )
    {
    	return ( 0 );
    }

    void * msg = reinterpret_cast<void *>(const_cast<u_int8_t *>(message));

    return ( sendMessage(msg, messageLength, 0) );
}


int 	
DatagramSocket::sendTo(
    const string    	    & 	message, 
    const TransportAddress  &	remoteAddress
)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( message.size() == 0 )
    {
    	return ( 0 );
    }

    void * msg = reinterpret_cast<void *>(const_cast<char *>(message.c_str()));

    return ( sendMessage(msg, message.size(), &remoteAddress) );
}


int 	
DatagramSocket::sendTo(
    const char    	    * 	message, 
    const TransportAddress  &	remoteAddress
)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    size_t messageLength = ( message ? strlen(message) : 0 );
    
    if ( messageLength == 0 )
    {
    	return ( 0 );
    }

    void * msg = reinterpret_cast<void *>(const_cast<char *>(message));

    return ( sendMessage(msg, messageLength, &remoteAddress) );
}


int 	
DatagramSocket::sendTo( 
    const vector<u_int8_t>  & 	message,
    const TransportAddress  &	remoteAddress
)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( message.size() == 0 )
    {
    	return ( 0 );
    }

    void * msg = reinterpret_cast<void *>(const_cast<u_int8_t *>(&message[0]));

    return ( sendMessage(msg, message.size(), &remoteAddress) );
}


int 	
DatagramSocket::sendTo( 
    const u_int8_t  	    * 	message,
    size_t  	    	    	messageLength,
    const TransportAddress  &	remoteAddress
)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( message == 0 || messageLength == 0 )
    {
    	return ( 0 );
    }

    void * msg = reinterpret_cast<void *>(const_cast<u_int8_t *>(message));

    return ( sendMessage(msg, messageLength, &remoteAddress) );
}


int 	
DatagramSocket::receive(string & message)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( !remoteAddr_ )
    {
    	return ( 0 );
    }
    
    void * msg = reinterpret_cast<void *>(const_cast<char *>(message.data()));

    int bytesReceived = recvMessage(msg, message.size(), 0);

    message.resize(bytesReceived);
    
    return ( bytesReceived );
}


int 	
DatagramSocket::receive(char * message, size_t messageCapacity)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( !remoteAddr_ || message == 0 || messageCapacity == 0 )
    {
    	return ( 0 );
    }
    
    void * msg = reinterpret_cast<void *>(message);

    int bytesReceived = recvMessage(msg, (messageCapacity > 1 ? messageCapacity-1 : 1 ), 0);
    
    if ( messageCapacity > 1 )
    {
    	message[bytesReceived] = '\0';
    }

    return ( bytesReceived );
}


int 	
DatagramSocket::receive(vector<u_int8_t> & message)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( !remoteAddr_ )
    {
    	return ( 0 );
    }
    
    message.clear();
    
    void * msg = reinterpret_cast<void *>(&message[0]);

    int bytesReceived = recvMessage(msg, message.size(), 0);

    return ( bytesReceived );
}


int 	
DatagramSocket::receive(u_int8_t * message, size_t messageCapacity)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( !remoteAddr_ || message == 0 || messageCapacity == 0 )
    {
    	return ( 0 );
    }
    
    void * msg = reinterpret_cast<void *>(message);

    int bytesReceived = recvMessage(msg, messageCapacity, 0);
    
    return ( bytesReceived );
}


int 	
DatagramSocket::receiveFrom(
    string  	    	    &	message, 
    TransportAddress  	    &	remoteAddress
)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    void * msg = reinterpret_cast<void *>(const_cast<char *>(message.data()));
   
    int bytesReceived = recvMessage(msg, message.size(), &remoteAddress);

    message.resize(bytesReceived);
    
    return ( bytesReceived );
}


int 	
DatagramSocket::receiveFrom(
    char  	    	    *	message, 
    size_t   	    	    	messageCapacity,
    TransportAddress  	    &	remoteAddress
)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( message == 0 || messageCapacity == 0 )
    {
    	return ( 0 );
    }
    
    void * msg = reinterpret_cast<void *>(message);
   
    int bytesReceived = recvMessage(msg, (messageCapacity > 1 ? messageCapacity-1 : 1 ), &remoteAddress);

    if ( messageCapacity > 1 )
    {    
    	message[bytesReceived] = '\0';
    }
    
    return ( bytesReceived );
}


int 	
DatagramSocket::receiveFrom( 
    vector<u_int8_t> 	    &	message,
    TransportAddress  	    &	remoteAddress
)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    message.clear();
    
    void * msg = reinterpret_cast<void *>(&message[0]);
   
    int bytesReceived = recvMessage(msg, message.size(), &remoteAddress);

    message.resize(bytesReceived);
    
    return ( bytesReceived );
}


int 	
DatagramSocket::receiveFrom( 
    u_int8_t	 	    *	message,
    size_t  	    	    	messageCapacity,
    TransportAddress  	    &	remoteAddress
)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( message == 0 || messageCapacity == 0 )
    {
    	return ( 0 );
    }
    
    void * msg = reinterpret_cast<void *>(message);
   
    return ( recvMessage(msg, messageCapacity, &remoteAddress) );
}


Sptr<TransportAddress>
DatagramSocket::getRemoteAddress() const
{
    return ( remoteAddr_ );
}		


ostream &           	
DatagramSocket::writeTo(ostream & out) const
{
    Socket::writeTo(out);
    
    if ( remoteAddr_ != 0 )
    {
    	out << ", remote = " << *remoteAddr_;
    }

    return ( out );
}


int
DatagramSocket::sendMessage(
    void    	    	    * 	message, 
    int     	    	    	messageLength, 
    const TransportAddress  *	addr
)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    const string    fn("DatagramSocket::send");
    VLog    	    log(fn);

    int bytesSent = 0;

    sockbuf_t   *   msg = static_cast<sockbuf_t *>(message);
        
    if ( addr )
    {
    	bytesSent = ::sendto(	fd_, 
	    	    	    	msg, 
				messageLength, 
				0, 
	    	    	    	addr->getAddress(), 
				addr->getAddressLength());
    }
    else
    {
    	bytesSent = ::send(	fd_, 
	    	    	      	msg, 
			      	messageLength, 
                                0);
    }
    
    if ( bytesSent < SUCCESS )
    {
    	int error = errno;
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without sending the packet. If the socket is set 
	// blocking we can get an EINTR here, without sending the packet.
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
DatagramSocket::recvMessage(
    void    	    	    *	message,
    int     	    	    	messageCapacity,
    TransportAddress	    *	addr
)
//throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    const string    fn("DatagramSocket::receive");
    VLog    	    log(fn);

    int bytesReceived = 0;

    sockbuf_t   *   msg = static_cast<sockbuf_t *>(message);
        
    if ( addr )
    {
    	AddressFamily	    af = addr->getAddressFamily();
	
	socklen_t   	    addrLength = addr->getAddressLength();
	
    	bytesReceived = ::recvfrom(	fd_, 
	    	    	    	    	msg, 
					messageCapacity, 
					0, 
	    	    	    	    	addr->getAddress(), 
					&addrLength);

    	// They gave us the wrong type of address. duh.
	//				
	if ( af != addr->getAddressFamily() )
	{
            throw Vocal::SystemException(fn + " on recvfrom(): incorrect address type", 
	    	    	    	__FILE__, __LINE__);
	}
    }
    else
    {
    	bytesReceived = ::recvfrom(	fd_, 
	    	    	    	    	msg, 
					messageCapacity, 
					0,
	    	    	    	    	0, 
					0);
    }

    if ( bytesReceived < SUCCESS )
    {
    	int error = errno;
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without receiving the packet. If the socket is set 
	// blocking we can get an EINTR here, without receiving the packet.
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
}
