#if !defined(IP6ADDRESS_DOT_H)
#define IP6ADDRESS_DOT_H

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


static const char* const IP6Address_hxx_Version = 
    "$Id: IP6Address.hxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "TransportAddress.hxx"
#include <netinet/in.h>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


#if defined(__linux__)

/** IP version 6 style address, extends TransportAddress.<br><br>
 *
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::Transport::IPAddress
 *  @see    Vocal::Transport::AddressFamily
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::SystemException
 */
class IP6Address : public Vocal::Transport::TransportAddress
{


    public:


    	/** Construct given the optional port number.
	 */
    	IP6Address(u_int16_t 	    	    port = 0);
	

    	/** Construct given a sockaddr_in6.
	 */
	IP6Address(const sockaddr_in6 &     in6Addr);


    	/** Copy constructor.
	 */
	IP6Address(const IP6Address 	    &);
		

    	/** Virtual destructor.
	 */	    
	virtual ~IP6Address();


    	/** Assignment operator
	 */	
	IP6Address & 	    	operator=(const IP6Address &);


    	/** Factory method. Clone the current address.
	 */
	virtual TransportAddress *  	clone() const;
		

    	/** Set the socket address.
	 */
    	virtual void	    	setAddress(sockaddr *);


	/** Set entire address by sockaddr_in6
	 */
	void	    	    	setIP6Address(const sockaddr_in6 &);
	

	/** Set port portion of the address.
	 */
	void	    	    	setPort(u_int16_t);
    

    	/** Get the address in the form of a sockaddr.
	 */	
    	virtual sockaddr *  	getAddress() const;


    	/** Get the native address.
	 */
    	const sockaddr_in6 &  	getIP6Address() const;


    	/** Get the length of the native address.
	 */
   	virtual socklen_t   	getAddressLength() const;
	

	/** Get the port portion of the address.
	 */
	u_int16_t   	    	getPort() const;


    	/** Update the address from the socket.
	 */
	virtual void	    	updateAddress(const Socket &)
	    	    	    	    throw ( Vocal::SystemException );

    	/** Equality relational operator
	 */
	bool	    	    	operator==(const IP6Address &) const;


    	/** Inequality relational operator
	 */
	bool	    	    	operator!=(const IP6Address &) const;


    	/** Less than relational operator
	 */
	bool	    	    	operator<(const IP6Address &) const;


    	/** Less than or equal relational operator
	 */
	bool	    	    	operator<=(const IP6Address &) const;


    	/** Greater than relational operator
	 */
	bool	    	    	operator>(const IP6Address &) const;


    	/** Greater than or equal to relational operator
	 */
	bool	    	    	operator>=(const IP6Address &) const;


        /** Clear the address
         */
        void                    clear();
        
	
	/** Write the address to a stream.
	 */
	ostream &	    	writeTo(ostream &) const;
	

    private:
    

    	mutable sockaddr_in6	in6Addr_;
};
#endif // defined(__linux__)


} // namespace Transport
} // namespace Vocal


#endif // !defined(IP6ADDRESS_DOT_H)
