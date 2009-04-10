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
#include "SocketOptions.hxx"
#include "Socket.hxx"
#include "TransportCommon.hxx"
#include "SystemException.hxx"
#include "IPAddress.hxx"
#include "VLog.hxx"
#include <cerrno>


using Vocal::Transport::SocketOptions;
using Vocal::Transport::Socket;
using Vocal::Transport::IPAddress;
using Vocal::Logging::VLog;
using namespace Vocal;


SocketOptions::SocketOptions(Socket & sock)
    :	socket_(sock)
{
}


SocketOptions::~SocketOptions()
{
}


void	    	
SocketOptions::keepAlive(bool on)
throw ( Vocal::SystemException )
{
    set(SOL_SOCKET, SO_KEEPALIVE, on);
}


bool	    	
SocketOptions::keepAlive() const
throw ( Vocal::SystemException )
{
    return ( get(SOL_SOCKET, SO_KEEPALIVE) );
}


void	    	
SocketOptions::bsdCompatible(bool on)
throw ( Vocal::SystemException )
{
    #if defined(__linux__)
    set(SOL_SOCKET, SO_BSDCOMPAT, on);
    #endif // defined(__linux__)
}


bool	    	
SocketOptions::bsdCompatible() const
throw ( Vocal::SystemException )
{
    #if defined(__linux__)
    return ( get(SOL_SOCKET, SO_BSDCOMPAT) );
    #else
    return ( false );
    #endif // defined(__linux__)
}


void	    	
SocketOptions::reuseAddress(bool on)
throw ( Vocal::SystemException )
{
    set(SOL_SOCKET, SO_REUSEADDR, on);
}


bool	    	
SocketOptions::reuseAddress() const		    
throw ( Vocal::SystemException )
{
    return ( get(SOL_SOCKET, SO_REUSEADDR) );
}
		

void	    	
SocketOptions::broadcast(bool on)
throw ( Vocal::SystemException )
{
    set(SOL_SOCKET, SO_BROADCAST, on);
}

		
bool	    	
SocketOptions::broadcast() const
throw ( Vocal::SystemException )
{
    return ( get(SOL_SOCKET, SO_BROADCAST) );
}
				    

void	    	
SocketOptions::linger(bool on, int seconds)
throw ( Vocal::SystemException )
{
    const string    fn("SocketOptions::linger");
    VLog    	    log(fn);

    ::linger  lin;

    lin.l_onoff = on;
    lin.l_linger = seconds;
    
    socklen_t	optionLength = sizeof(lin);    

    if  ( setsockopt(socket_.getFD(), SOL_SOCKET, SO_LINGER, 
    	    	    #if defined(__sun) || defined(WIN32)
		    (char *)
		    #endif // defined(__sun)
    	    	    &lin, 
		    optionLength)
    	    < SUCCESS 
	)
    {
    	int error = errno;
	
    	throw Vocal::SystemException(fn + " on setsockopt(): " + strerror(error),
	    	    	    __FILE__, __LINE__, error);
    }

    VDEBUG(log) << fn << ": option = SO_LINGER, value = " << (on ? "on" : "off") 
    	    	<< ", seconds = " << (on ? seconds : 0)
		<< VDEBUG_END(log);
}


bool	    	
SocketOptions::linger(int & seconds) const
throw ( Vocal::SystemException )
{
    const string    fn("SocketOptions::linger");
    VLog    	    log(fn);

    ::linger  lin;
    socklen_t	optionLength = sizeof(lin);    

    if  ( getsockopt(socket_.getFD(), SOL_SOCKET, SO_LINGER, 
    	    	    #if defined(__sun) || defined(WIN32)
		    (char *)
		    #endif // defined(__sun)
    	    	    &lin, 
		    &optionLength) 
    	    < SUCCESS 
	)
    {
    	int error = errno;
	
    	throw Vocal::SystemException(fn + " on setsockopt(): " + strerror(error),
	    	    	    __FILE__, __LINE__, error);
    }

    int on = lin.l_onoff;
    seconds = lin.l_linger;
    
    VDEBUG(log) << fn << ": option = SO_LINGER, value = " << (on ? "on" : "off") 
    	    	<< ", seconds = " << (on ? seconds : 0)
		<< VDEBUG_END(log);

    return ( on ? true : false );
}
		

void	    	
SocketOptions::joinMulticastGroup(IPAddress & mcastAddr)
throw ( Vocal::SystemException )
{
    const string    fn("SocketOptions::joinMulticastGroup");
    VLog    	    log(fn);

    if ( socket_.getAddressFamily() != AddressFamily(AF_INET) )
    {
    	return;
    }

    #if !defined(WIN32)

    #if defined(__linux__)
    ip_mreqn	mcReq;
    
    mcReq.imr_multiaddr = mcastAddr.getIPAddress().sin_addr;
    mcReq.imr_address = ((sockaddr_in *)(socket_.getLocalAddress().getAddress()))->sin_addr;
    mcReq.imr_ifindex = 0;
    #else
    ip_mreq  	mcReq;

    mcReq.imr_multiaddr = mcastAddr.getIPAddress().sin_addr;
    mcReq.imr_interface = ((sockaddr_in *)(socket_.getLocalAddress().getAddress()))->sin_addr;
    #endif // defined(__linux__)

    socklen_t	optionLength = sizeof(mcReq); 
        
    if  ( setsockopt(socket_.getFD(), 
    	    	    IPPROTO_IP,
		    IP_ADD_MEMBERSHIP, 
    	    	    #if defined(__sun) || defined(WIN32)
		    (char *)
		    #endif // defined(__sun)
    	    	    &mcReq, 
		    optionLength) < SUCCESS 
	)
    {
    	int error = errno;
	
    	throw Vocal::SystemException(fn + " on setsockopt(): " + strerror(error),
	    	    	    __FILE__, __LINE__, error);
    }

    VDEBUG(log) << fn << ": option = IP_ADD_MEMBERSHIP"
    	    	<< ", local addr = " << socket_.getLocalAddress()
    	    	<< ", mapped to multicast group = " << mcastAddr
		<< VDEBUG_END(log);

    #else // defined WIN32

    // TODO
    assert(0);

    #endif
}


void	    	
SocketOptions::leaveMulticastGroup(IPAddress & mcastAddr)
throw ( Vocal::SystemException )
{
    const string    fn("SocketOptions::leaveMulticastGroup");
    VLog    	    log(fn);

    if ( socket_.getAddressFamily() != AddressFamily(AF_INET) )
    {
    	return;
    }

    #if !defined(WIN32)

    #if defined(__linux__)
    ip_mreqn	mcReq;
    
    mcReq.imr_multiaddr = mcastAddr.getIPAddress().sin_addr;
    mcReq.imr_address = ((sockaddr_in *)(socket_.getLocalAddress().getAddress()))->sin_addr;
    mcReq.imr_ifindex = 0;
    #else
    ip_mreq  	mcReq;

    mcReq.imr_multiaddr = mcastAddr.getIPAddress().sin_addr;
    mcReq.imr_interface = ((sockaddr_in *)(socket_.getLocalAddress().getAddress()))->sin_addr;
    #endif // defined(__linux__)

    socklen_t	optionLength = sizeof(mcReq); 
        
    if  ( setsockopt(socket_.getFD(), 
    	    	    IPPROTO_IP,
		    IP_DROP_MEMBERSHIP, 
    	    	    #if defined(__sun) || defined(WIN32)
		    (char *)
		    #endif // defined(__sun)
    	    	    &mcReq, 
		    optionLength) < SUCCESS 
	)
    {
    	int error = errno;
	
    	throw Vocal::SystemException(fn + " on setsockopt(): " + strerror(error),
	    	    	    __FILE__, __LINE__, error);
    }

    VDEBUG(log) << fn << ": option = IP_DROP_MEMBERSHIP"
    	    	<< ", local addr = " << socket_.getLocalAddress()
    	    	<< ", leaving multicast group = " << mcastAddr
		<< VDEBUG_END(log);

    #else // defined(WIN32)

    // TODO
    assert(0);
    
    #endif
}


void	    	
SocketOptions::set(int level, int optname, bool opt)
throw ( Vocal::SystemException )
{
    const string    fn("SocketOptions::set");
    VLog    	    log(fn);
    
    int     	    option = opt;
    socklen_t	    optionLength = sizeof(option);
    
    if  ( setsockopt(socket_.getFD(), 
    	    	     level, 
		     optname, 
		     #if defined(__sun) || defined(WIN32)
		     (char *)
		     #endif // defined(__sun)
		     &option, 
		     optionLength) 
    	    < SUCCESS 
	)
    {
    	int error = errno;
	
    	throw Vocal::SystemException(fn + " on setsockopt(): " + strerror(error),
	    	    	    __FILE__, __LINE__, error);
    }


    VDEBUG(log) << fn << ": option = ";
    
    if ( level == SOL_SOCKET )
    {
        switch ( optname )
        {
    	    case SO_KEEPALIVE:
	    {
	    	VDEBUG_CONT(log) << "SO_KEEPALIVE";
	    	break;
    	    }
	    #if defined(__linux__)
	    case SO_BSDCOMPAT:
	    {
	    	VDEBUG_CONT(log) << "SO_BSDCOMPAT";
	    	break;
    	    }
	    #endif // defined(__linux__)
	    case SO_REUSEADDR:
	    {
	    	VDEBUG_CONT(log) << "SO_REUSEADDR";
	    	break;
    	    }
	    case SO_BROADCAST:
	    {
	    	VDEBUG_CONT(log) << "SO_BROADCAST";
	    	break;
    	    }
	    default:
	    {
	    	VDEBUG_CONT(log) << "Unknown option";
		break;
    	    }
	}
	
    }
    else
    {
    	VDEBUG_CONT(log) << "Unknown option";
    }
        
    VDEBUG_CONT(log) << ", value = " << (opt ? "true" : "false")
    	<< VDEBUG_END(log);
}


bool
SocketOptions::get(int level, int optname) const
throw( Vocal::SystemException )
{
    const string    fn("SocketOptions::get");
    VLog    	    log(fn);

    int     	    option = 0;
    socklen_t	    optionLength = sizeof(option);
    
    if  ( getsockopt(socket_.getFD(), 
    	    	     level, 
		     optname, 
		     #if defined(__sun) || defined(WIN32)
		     (char *)
		     #endif // defined(__sun)
		     &option, 
		     &optionLength) 
    	    < SUCCESS 
	)
    {
    	int error = errno;
	
    	throw Vocal::SystemException(fn + " on setsockopt(): " + strerror(error),
	    	    	    __FILE__, __LINE__, error);
    }

    VDEBUG(log) << fn << ": option = ";
    
    if ( level == SOL_SOCKET )
    {
        switch ( optname )
        {
    	    case SO_KEEPALIVE:
	    {
	    	VDEBUG_CONT(log) << "SO_KEEPALIVE";
	    	break;
    	    }
	    #if defined(__linux__)
	    case SO_BSDCOMPAT:
	    {
	    	VDEBUG_CONT(log) << "SO_BSDCOMPAT";
	    	break;
    	    }
	    #endif // defined(__linux__)
	    case SO_REUSEADDR:
	    {
	    	VDEBUG_CONT(log) << "SO_REUSEADDR";
	    	break;
    	    }
	    case SO_BROADCAST:
	    {
	    	VDEBUG_CONT(log) << "SO_BROADCAST";
	    	break;
    	    }
	    default:
	    {
	    	VDEBUG_CONT(log) << "Unknown option";
		break;
    	    }
	}
	
    }
    else
    {
    	VDEBUG_CONT(log) << "Unknown option";
    }
        
    VDEBUG_CONT(log) << ", value = " << (option ? "true" : "false")
    	<< VDEBUG_END(log);

    return ( option ? true : false );
}
