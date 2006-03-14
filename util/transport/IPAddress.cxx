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


static const char* const IPAddress_cxx_Version = 
    "$Id: IPAddress.cxx,v 1.3 2006/03/14 00:20:07 greear Exp $";


#include "global.h"
#include "IPAddress.hxx"
#include "TransportCommon.hxx"
#include "SystemException.hxx"
#include "Socket.hxx"
#include "VLog.hxx"
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <strstream>

#ifndef __WIN32__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

using Vocal::Transport::IPAddress;
using Vocal::Transport::TransportAddress;
using Vocal::Transport::Socket;
using Vocal::Logging::VLog;


IPAddress::IPAddress(u_int16_t	port)
    :   separationChar_(':')
{
    setAddressFamily(AF_INET);
    inAddr_.sin_port 	    	= htons(port);
    inAddr_.sin_addr.s_addr	= htonl(INADDR_ANY);
}


IPAddress::IPAddress(
    const char	    	*   ip,
    u_int16_t 	    	    port
)   
    :   separationChar_(':')
{
    setAddressFamily(AF_INET);
    inAddr_.sin_port = htons(port);
    setIP(ip);
}


IPAddress::IPAddress(
    const u_int8_t 	*   ip,
    u_int16_t 	    	    port
)
    :   separationChar_(':')
{
    setAddressFamily(AF_INET);
    inAddr_.sin_port = htons(port);
    setIP(ip);
}


IPAddress::IPAddress(
    u_int32_t	    	    ip,
    u_int16_t 	    	    port
)
    :   separationChar_(':')
{
    setAddressFamily(AF_INET);
    inAddr_.sin_port 	    	= htons(port);
    inAddr_.sin_addr.s_addr	= htonl(ip);
}


IPAddress::IPAddress(const sockaddr_in &   inAddr)
    :	inAddr_(inAddr),
        separationChar_(':')
{
    setAddressFamily(AF_INET);
}


IPAddress::IPAddress(const IPAddress & src)
    :	inAddr_(src.inAddr_),
        separationChar_(src.separationChar_)
{
    setAddressFamily(AF_INET);
}
			    

IPAddress::~IPAddress()
{
}


IPAddress &
IPAddress::operator=(const IPAddress & src)
{
    if ( this != &src )
    {
    	setIPAddress(src.inAddr_);
    }
    return ( *this );
}


TransportAddress *
IPAddress::clone() const
{
    return ( new IPAddress(*this) );
}


void	    	    	    	
IPAddress::setAddress(sockaddr * addr)
{
    if ( addr->sa_family != AF_INET )
    {
    	VLog	log;
	VWARN(log)  << "IPAddress::setAddress: incorrect address family = "
	    	    << addr->sa_family << VWARN_END(log);
    	return;
    }

    inAddr_ = *reinterpret_cast<sockaddr_in *>(addr);
}


void	    	    
IPAddress::setIPAddress(const sockaddr_in & src)
{
    inAddr_ = src;
    setAddressFamily(AF_INET);
}


void	    	    
IPAddress::setIP(const char * ip)
{
    size_t  length = ip ? strlen(ip) : 0;

    if ( length == 0 )
    {
        inAddr_.sin_addr.s_addr = htonl(INADDR_ANY);
        inAddr_.sin_port = htons(0);

	return;
    }

    // Copy it, since we need to modify it.
    //    
    char *  ipCopy = new char[length+1];
    strcpy(ipCopy, ip);
        
    // See if we have a string in the format a.b.c.d:port. We will search 
    // until we find a separator character, being a member of the character
    // set " \t:;,/\\". We will then replace that with a 0 and then
    // search for the first digit.
    //
    string  	    	searchString(ip);
    string::size_type 	pos = searchString.find_first_of(" \t:;,/\\");

    bool foundPortSeparator = false;
        
    if ( pos != string::npos )
    {
        separationChar_ = searchString[pos];
        
        foundPortSeparator = true;
        
    	// Null terminate the dotted decimal.
	//
	ipCopy[pos] = '\0';
	char *	position = &ipCopy[pos+1];
	
	// Advance until we find the end or we find a digit.
	//
	while ( *position && !isdigit(*position) )
	{
	    position++;
	}

    	// See if we have a number.
	//	
	if ( *position )
	{
	    char * endPtr = 0;
	    u_int16_t newPort = (u_int16_t)strtoul(position, &endPtr, 10);

	    if ( endPtr != position )
	    {
    	    	inAddr_.sin_port = htons(newPort);
	    }
	}
    }

    searchString = ipCopy;
    pos = searchString.find_first_of(".");

    // We have dotted decimal
    //
    if ( foundPortSeparator || pos != string::npos )
    {
        #if defined(__linux__)
        if ( !inet_aton(ipCopy, &inAddr_.sin_addr) )
        #else
        if ( (inAddr_.sin_addr.s_addr = inet_addr(ipCopy)) != (in_addr_t)-1 )
        #endif
        {
            inAddr_.sin_addr.s_addr = htonl(INADDR_ANY);
        }
    }
    
    // No dot. Assume it's a port.
    //
    else
    {
        char * endPtr = 0;
	u_int16_t newPort = (u_int16_t)strtoul(ipCopy, &endPtr, 10);

        inAddr_.sin_addr.s_addr = htonl(INADDR_ANY);
        inAddr_.sin_port = htons(newPort);
    }
    
    delete [] ipCopy;
}


void	    	    
IPAddress::setIP(const string & ip)
{
    setIP(ip.c_str());
}


void	    	    
IPAddress::setIP(const u_int8_t * ip)
{
    // We have a 4 byte array that better be in network order.
    //
    if ( ip )
    {
        memcpy(&inAddr_.sin_addr.s_addr, ip, 4);
    }
    else
    {
    	inAddr_.sin_addr.s_addr = htonl(INADDR_ANY);
    }
}


void	    	    
IPAddress::setIP(u_int32_t ip)
{
    inAddr_.sin_addr.s_addr = htonl(ip);
}
	

void	    	    
IPAddress::setPort(u_int16_t port)
{
    inAddr_.sin_port = htons(port);
}


string	    	    	
IPAddress::getIPAsString() const
{
    char    buffer[18];
    string  ipAsString(inetNtoa(inAddr_.sin_addr, buffer));
    return ( ipAsString );
}


void 
IPAddress::getIPAsOctetString(u_int8_t* dst) const
{
    memcpy(dst, &inAddr_.sin_addr.s_addr, 4);
}


u_int32_t 
IPAddress::getIPAsULong() const
{
    return ntohl(inAddr_.sin_addr.s_addr);
}


sockaddr *  	    	
IPAddress::getAddress() const
{
    return ( reinterpret_cast<sockaddr *>(&inAddr_) );
}


const sockaddr_in &	    
IPAddress::getIPAddress() const
{
    return ( inAddr_ );
}


socklen_t
IPAddress::getAddressLength() const
{
    return ( sizeof(inAddr_) );
}


string	    	    	
IPAddress::getIPAndPortAsString() const
{
    char    	buf[32];
    strstream	addr(buf, 31);
    addr << *this << ends;
    string  	ipAndPort(addr.str());
    
    return ( ipAndPort );
}


u_int16_t   	    
IPAddress::getPort() const
{
    return ( ntohs(inAddr_.sin_port) );
}


void	    	    
IPAddress::updateAddress(const Socket & sock) 
throw ( Vocal::SystemException )
{
    const string    fn = "IPAddress::updateAddress";
    VLog    	    log(fn);

    sockaddr_in     newAddress;
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

    setIPAddress(newAddress);
    
    VVERBOSE(log)   << (fn + ": ")  << *this 
    	    	    << ", from: "   << sock
    	    	    << VVERBOSE_END(log);
}


bool	    	    
IPAddress::operator==(const IPAddress & rhs) const
{
    return 
    ( 	this == &rhs
    ||	(   getAddressFamily()      == rhs.getAddressFamily()
    	&&  inAddr_.sin_addr.s_addr == rhs.inAddr_.sin_addr.s_addr
        &&  inAddr_.sin_port 	    == rhs.inAddr_.sin_port
	)
    );
}


bool	    	    
IPAddress::operator!=(const IPAddress & rhs) const
{
    return ( !operator==(rhs) );
}


bool	    	    
IPAddress::operator<(const IPAddress & rhs) const
{
    u_int32_t 	leftIp  = ntohl(inAddr_.sin_addr.s_addr),
    	    	rightIp = ntohl(rhs.inAddr_.sin_addr.s_addr);
		
    u_int16_t 	leftPort  = ntohs(inAddr_.sin_port),
    	    	rightPort = ntohs(rhs.inAddr_.sin_port);
		
    return 
    ( 	this != &rhs
    &&	(   getAddressFamily() < rhs.getAddressFamily()
    	||  leftIp < rightIp
    	||  leftPort < rightPort
	)
    );
}


bool	    	    
IPAddress::operator<=(const IPAddress & rhs) const
{
    return
    (	operator==(rhs)
    &&	operator<(rhs)
    );
}


bool	    	    
IPAddress::operator>(const IPAddress & rhs) const
{
    return ( !operator<=(rhs) );
}


bool	    	    
IPAddress::operator>=(const IPAddress & rhs) const
{
    return ( !operator<(rhs) );
}


bool	    	    	
IPAddress::isAny() const
{
    return ( *this == IPAddress() );
}


void
IPAddress::clear()
{
    inAddr_.sin_port 	    	= htons(0);
    inAddr_.sin_addr.s_addr	= htonl(INADDR_ANY);
}


ostream &	    
IPAddress::writeTo(ostream & out) const
{
    return 
    (
    	out << getIPAsString() << separationChar_ << getPort()
    );
}


char *	    	    	
IPAddress::inetNtoa(const struct in_addr & in, char * buffer) const
{
    u_int8_t * bytes = (u_int8_t *) &in;
    #ifndef WIN32
    snprintf(buffer, 18, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    #else
    sprintf(buffer, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    #endif

    return ( buffer );
}
