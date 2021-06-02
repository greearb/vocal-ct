#if !defined(VOCAL_TRANSPORTADDRESS_HXX)
#define VOCAL_TRANSPORTADDRESS_HXX

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


#include "TransportCommon.hxx"
#include "Writer.hxx"
#include "AddressFamily.hxx"
#include "SystemException.hxx"
#include <RCObject.hxx>

/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class Socket;


/** Interface class for all forms of transport addresses.<br><br>
 *
 *  @see    Vocal::Transport::IPAddress
 *  @see    Vocal::Transport::AddressFamily
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::SystemException
 */
class TransportAddress : public Vocal::IO::Writer
{

    protected:


    	/** Default construction.
	 */
    	TransportAddress();


    public:				


    	/** Factory method to create a transport address given
	 *  the address family or clone the current address.
	 */
    	static	TransportAddress *  create(int addressFamily);


    	/** Factory method to clone this transport address.
	 */
	virtual TransportAddress *  clone() const = 0;


        /** Clear the address
         */
        virtual void        clear() = 0;


    protected:


    	/** Set the socket address.
	 */
    	virtual void	    	    setAddress(sockaddr *) = 0;
	

	/** Set the address family.
	 */
	void	    	    	    setAddressFamily(int);
	

    public:


    	/** Get the address in the form of a sockaddr.
	 */	
    	virtual sockaddr *  	    getAddress() const = 0;


    	/** Get the address length.
	 */	
   	virtual socklen_t   	    getAddressLength() const = 0;
	

	/** Get the address family.
	 */
	AddressFamily	    	    getAddressFamily() const;


    	/** Update the address from the socket.
	 */
        virtual void	    	updateAddress(const Socket &) = 0; // may throw exception

    private:

    	/** Copying suppressed.
	 */
	TransportAddress(const TransportAddress &);


    	/** Copying suppressed.
	 */
	TransportAddress & operator=(const TransportAddress &);
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_TRANSPORTADDRESS_HXX)
