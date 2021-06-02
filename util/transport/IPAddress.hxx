#if !defined(IPADDRESS_DOT_H)
#define IPADDRESS_DOT_H

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


#include "TransportAddress.hxx"
#ifndef __MINGW32__
#include <netinet/in.h>
#endif
#include <string>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


class SystemException;


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


/** IP version 4 style address, extends TransportAddress.<br><br>
 *
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::Transport::IP6Address
 *  @see    Vocal::Transport::AddressFamily
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::SystemException
 */
class IPAddress : public Vocal::Transport::TransportAddress
{
    public:


    	/** Construct given the optional port number.
	 */
	IPAddress(u_int16_t 	    	port = 0);
	

    	/** Construct given the ip in dotted decimal, C string format,
	 *  and an optional port.
	 */
	IPAddress(const char 	    *	ip,
		  u_int16_t 	    	port = 0);


    	/** Construct given the ip in an array of octets, each octet
	 *  representing a portion of the address and an optional port.
	 *  <br><br>
	 *
	 *  Note that the size of the array must be at least 4 bytes long,
	 *  otherwise the operation is unknown, aka don't call me if
	 *  it crashes in this code thanks to a bogus array that you passed
	 *  in.
	 */
	IPAddress(const u_int8_t    *   ip,
		  u_int16_t 	    	port = 0);


    	/** Construct given an ip as a host ordered long, and a port.
	 */
	IPAddress(u_int32_t    	    	ip,
		  u_int16_t 	    	port);


    	/** Construct given a sockaddr_in.
	 */
	IPAddress(const sockaddr_in &   inAddr);


    	/** Copy constructor.
	 */
	IPAddress(const IPAddress &);
			    

	/** Assignment operator
	 */
	IPAddress & 	    	operator=(const IPAddress &);


    	/** Factory method. clone the current address.
	 */
	virtual TransportAddress  *	clone() const;
		

    	/** Set the socket address.
	 */
    	virtual void	    	setAddress(sockaddr *);


	/** Set the address by sockaddr_in
	 */
	void	    	    	setIPAddress(const sockaddr_in &);
	
	
	/** Set IP portion of address by dotted decimal, C string.
	 */
	void	    	    	setIP(const char *);


	/** Set IP portion of address by dotted decimal, C string.
	 */
	void	    	    	setIP(const string &);


	/** Set IP portion of address by octet sting. Must be at least
	 *  4 octets long.
	 */
	void	    	    	setIP(const u_int8_t *longForm);


	/** Set IP portion of address by host ordered unsigned long.
	 */
	void	    	    	setIP(u_int32_t);
	

	/** Set port portion of address.
	 */
	void	    	    	setPort(u_int16_t);
    

	/** Get IP portion of the address by dotted decimal notation.
	 */
	string	    	    	getIPAsString() const;


	/** Get IP portion of the address by octet sting. 
	 *  Assuming dst points to 4-byte memory area.
	 */
	void     	        getIPAsOctetString(u_int8_t* dst) const;


	/** Get IP portion of the address as host ordered unsigned long.
	 */
	u_int32_t               getIPAsULong() const;


	/** Get IP and port  by dotted decimal notation followed by a colon
	 *  followed by a port, e.g. "192.168.5.23:1719".
	 */
    	string	    	    	getIPAndPortAsString() const;
		

    	/** Get the address in the form of a sockaddr.
	 */	
    	virtual sockaddr *  	getAddress() const;


    	/** Get the entire address as a sockaddr_in.
	 */
    	const sockaddr_in &	getIPAddress() const;


    	/** Get the native sockaddr's length.
	 */
   	virtual socklen_t   	getAddressLength() const;
	

	/** Get the port portion of the address.
	 */
	u_int16_t   	    	getPort() const;


    	/** Update the address from the socket.
	 */
	virtual void	    	updateAddress(const Socket &); // may throw exception

    	/** Equality relational operator
	 */
	bool	    	    	operator==(const IPAddress &) const;


    	/** Inequality relational operator
	 */
	bool	    	    	operator!=(const IPAddress &) const;


    	/** Less than relational operator
	 */
	bool	    	    	operator<(const IPAddress &) const;


    	/** Less than  or equal to relational operator
	 */
	bool	    	    	operator<=(const IPAddress &) const;


    	/** Greater than relational operator
	 */
	bool	    	    	operator>(const IPAddress &) const;


    	/** Greater than or equal to relational operator
	 */
	bool	    	    	operator>=(const IPAddress &) const;


    	/** Returns true if the addr portion is INADDR_ANY and the
    	 *  port is zero.
	 */
    	bool	    	    	isAny() const;
	

        /** Clears the ip address
         */
        void                    clear();
        
        
    	/** Print the address to a stream.
	 */	
	ostream &	    	writeTo(ostream &) const;
	

    private:

    	char *	    	    	inetNtoa(const struct in_addr & in, char *) const;
	    
    	mutable sockaddr_in 	inAddr_;
        
        char                    separationChar_;
};


} // namespace Transport
} // namespace Vovida


#endif // !defined(IPADDRESS_DOT_H)
