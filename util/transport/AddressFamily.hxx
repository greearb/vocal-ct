#if !defined(ADDRESS_FAMILY_DOT_H)
#define ADDRESS_FAMILY_DOT_H

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


static const char* const AddressFamily_hxx_Version = 
    "$Id: AddressFamily.hxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#if defined(__FreeBSD__) || defined(__APPLE__) || defined(__OpenBSD__)
#include <sys/types.h>
#endif

#include <sys/socket.h>
#include "Writer.hxx"

/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{

/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


/** Abstracts AF_* (or PF_*) as defined in sys/socket.h.<br><br>
 *
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::IO::Writer
 */
class AddressFamily : public Vocal::IO::Writer
{
    public:
    	
    	/** Construct by AF_* defined in sys/socket.h. Default constructs
	 *  to AF_INET.
	 */
    	AddressFamily(int addressFamily = AF_INET);

    	/** Copy constructor.
	 */
    	AddressFamily(const AddressFamily &);
	
    	/** Destructor.
	 */
	virtual ~AddressFamily();

    	/** Assignment operator.
	 */
    	AddressFamily &     operator=(const AddressFamily &);
	
    	/** Accessor.
	 */
	int  	    	    operator()() const;
	
    	/** Mutator.
	 */
    	void	    	    operator()(int);

    	/** Equality relational operator.
	 */
	bool	    	    operator==(const AddressFamily &) const;

    	/** Inequality relational operator.
	 */
	bool	    	    operator!=(const AddressFamily &) const;

    	/** Less than relational operator.
	 */
	bool	    	    operator<(const AddressFamily &) const;

    	/** Less or equal to than relational operator.
	 */
	bool	    	    operator<=(const AddressFamily &) const;

    	/** Greater than relational operator.
	 */
	bool	    	    operator>(const AddressFamily &) const;

    	/** Greater than or equal to relational operator.
	 */
	bool	    	    operator>=(const AddressFamily &) const;
	
    	/** Write to ostream.
	 */
	ostream &           writeTo(ostream &) const;
	
    private:

    	int  	    	    addressFamily_;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(ADDRESS_FAMILY_DOT_H)
