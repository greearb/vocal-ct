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


#warning "Dead code."

#if 0

#if defined(__linux__)


#include "global.h"
#include "IP6Address.hxx"
#include "TransportCommon.hxx"
#include "SystemException.hxx"
#include "Socket.hxx"
#include "VLog.hxx"
#include <iomanip>
#include <cerrno>


using Vocal::Transport::IP6Address;
using Vocal::Transport::TransportAddress;
using Vocal::Transport::Socket;
using Vocal::Logging::VLog;


IP6Address::IP6Address(u_int16_t    port)
{
    setAddressFamily(AF_INET6);
    in6Addr_.sin6_port 	    	= htons(port);      // Transport layer port #
    in6Addr_.sin6_flowinfo  	= 0;	    	    // IPv6 flow information
    in6Addr_.sin6_addr	    	= in6addr_any;      // IPv6 address 
}


IP6Address::IP6Address(const sockaddr_in6 &     in6Addr)
    :	in6Addr_(in6Addr)
{
    setAddressFamily(AF_INET6);
}


IP6Address::IP6Address(const IP6Address & src)
    :	in6Addr_(src.in6Addr_)
{
    setAddressFamily(AF_INET6);
}


IP6Address::~IP6Address()
{
}


IP6Address & 	    	
IP6Address::operator=(const IP6Address & src)
{
    if ( this != &src )
    {
    	setIP6Address(src.in6Addr_);
    }
    return ( *this );
}
	

TransportAddress *
IP6Address::clone() const
{
    return ( new IP6Address(*this) );
}


void	    	
IP6Address::setAddress(sockaddr * addr)
{
    if ( addr->sa_family != AF_INET6 )
    {
    	VLog	log;
	VWARN(log)  << "IP6Address::setAddress: incorrect address family = "
	    	    << addr->sa_family << VWARN_END(log);
    	return;
    }

    in6Addr_ = *reinterpret_cast<sockaddr_in6 *>(addr);
}


void	    	    	
IP6Address::setIP6Address(const sockaddr_in6 & in6Addr)
{
    in6Addr_ = in6Addr;
    setAddressFamily(AF_INET6);
}


void	    	    	
IP6Address::setPort(u_int16_t port)
{
    in6Addr_.sin6_port = htons(port);
}


sockaddr *  	
IP6Address::getAddress() const
{
    return ( reinterpret_cast<sockaddr *>(&in6Addr_) );
}


const sockaddr_in6 &	    	
IP6Address::getIP6Address() const
{
    return ( in6Addr_ );
}


socklen_t  	    	
IP6Address::getAddressLength() const
{
    return ( sizeof(in6Addr_) );
}


u_int16_t   	    	
IP6Address::getPort() const
{
    return ( ntohs(in6Addr_.sin6_port) );
}
	

void	    	    
IP6Address::updateAddress(const Socket & sock)
throw ( Vocal::SystemException )
{
    const string    fn("IP6Address::updateAddress");
    VLog    	    log(fn);

    sockaddr_in6    newAddress;
    socklen_t	    newAddressLength = sizeof(newAddress);
    
    if	(   getsockname(sock.getFD(), 
    	    	    	(sockaddr *)&newAddress, 
		    	&newAddressLength)
	    < SUCCESS 
	)
    {
    	throw Vocal::SystemException(fn + " on getsockname(): " + strerror(errno), 
	    	    	    	 __FILE__, __LINE__, errno);
    }

    setIP6Address(newAddress);

    VDEBUG(log) << (fn + ": ") << *this 
    	    	<< ", from: " << sock
    	    	<< VDEBUG_END(log);
}

bool	    	    	
IP6Address::operator==(const IP6Address & rhs) const
{
    return 
    (	this == &rhs
    ||	(   getAddressFamily() == rhs.getAddressFamily()
    	&&  IN6_ARE_ADDR_EQUAL(&in6Addr_.sin6_addr, &rhs.in6Addr_.sin6_addr)
	&&  in6Addr_.sin6_port == rhs.in6Addr_.sin6_port
	)
    );
}


bool	    	    	
IP6Address::operator!=(const IP6Address & rhs) const
{
    return ( !operator==(rhs) );
}


bool	    	    	
IP6Address::operator<(const IP6Address & rhs) const
{
    // TEMP CODE: This should be here until the operating system / socket
    // stack has implemented similar functionality.
    //
    #define VOVIDA_IN6_IS_ADDR_LESS_THAN(a,b) \
	((((uint32_t *) (a))[0] < ((uint32_t *) (b))[0]) || \
	 (((uint32_t *) (a))[1] < ((uint32_t *) (b))[2]) || \
	 (((uint32_t *) (a))[2] < ((uint32_t *) (b))[1]) || \
	 (((uint32_t *) (a))[3] < ((uint32_t *) (b))[3]))

    uint16_t	leftPort  = ntohs(in6Addr_.sin6_port),
    	    	rightPort = ntohs(rhs.in6Addr_.sin6_port);
    return 
    (	this != &rhs
    &&	(   getAddressFamily() < rhs.getAddressFamily()
    	||  VOVIDA_IN6_IS_ADDR_LESS_THAN(&in6Addr_.sin6_addr, &rhs.in6Addr_.sin6_addr)
	||  leftPort < rightPort
	)
    );

    #undef VOVIDA_IN6_IS_ADDR_LESS_THAN
}


bool	    	    	
IP6Address::operator<=(const IP6Address & rhs) const
{
    return
    (	operator==(rhs)
    &&	operator<(rhs)
    );
}


bool	    	    	
IP6Address::operator>(const IP6Address & rhs) const
{
    return ( !operator<=(rhs) );
}


bool	    	    	
IP6Address::operator>=(const IP6Address & rhs) const
{
    return ( !operator<(rhs) );
}


void
IP6Address::clear()
{
    in6Addr_.sin6_port 	    	= htons(0);      // Transport layer port #
    in6Addr_.sin6_flowinfo  	= 0;	    	    // IPv6 flow information
    in6Addr_.sin6_addr	    	= in6addr_any;      // IPv6 address 
}


std::ostream &	    	
IP6Address::writeTo(std::ostream & out) const
{
    // TEMP CODE: This should be here until the operating system / socket
    // stack has implemented similar functionality.
    //
    AddressFamily   af = getAddressFamily();

    return 
    (    
        out << af << ":"
    	    << hex << ntohs(in6Addr_.sin6_addr.s6_addr16[0]) << ":"
    	    << hex << ntohs(in6Addr_.sin6_addr.s6_addr16[1]) << ":"
    	    << hex << ntohs(in6Addr_.sin6_addr.s6_addr16[2]) << ":"
    	    << hex << ntohs(in6Addr_.sin6_addr.s6_addr16[3]) << ":"
    	    << hex << ntohs(in6Addr_.sin6_addr.s6_addr16[4]) << ":"
    	    << hex << ntohs(in6Addr_.sin6_addr.s6_addr16[5]) << ":"
    	    << hex << ntohs(in6Addr_.sin6_addr.s6_addr16[6]) << ":"
    	    << hex << ntohs(in6Addr_.sin6_addr.s6_addr16[7]) << "/"
    	    << dec << ntohs(in6Addr_.sin6_port)
    );
}

#endif // defined(__linux__)
#endif
