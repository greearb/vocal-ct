#if !defined(SOCKET_TYPE_DOT_H)
#define SOCKET_TYPE_DOT_H

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


static const char* const SocketType_hxx_Version = 
    "$Id: SocketType.hxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "Writer.hxx"

#if defined(__FreeBSD__) || defined(__APPLE__) || defined(__OpenBSD__)
#include <sys/types.h>
#endif

#include <sys/socket.h>
#include <string>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


using std::string;


/** Wrapper for SOCK_* types defined in sys/socket.h.<br><br>
 *
 *  @see    Vocal::Transport::Socket
 */
class SocketType : public Vocal::IO::Writer
{
    public:

    	/** Construct by sys/socket.h SOCK_* manifest constant.
	 */
	SocketType(int type = SOCK_DGRAM);


    	/** Copy constructor.
	 */
	SocketType(const SocketType &);
	

    	/** Virtual destructor.
	 */
	virtual ~SocketType();


    	/** Assignment operator
	 */
    	SocketType &	    operator=(const SocketType &);
	

    	/** Accessor
	 */
	int  	    	    operator()();
	

    	/** Mutator
	 */
    	void	    	    operator()(int);


    	/** Equality relational operator
	 */
	bool	    	    operator==(const SocketType &) const;


    	/** Inequality relational operator
	 */
	bool	    	    operator!=(const SocketType &) const;


    	/** Less than relational operator
	 */
	bool	    	    operator<(const SocketType &) const;


    	/** Less than or equal relational operator
	 */
	bool	    	    operator<=(const SocketType &) const;


    	/** Greater than relational operator
	 */
	bool	    	    operator>(const SocketType &) const;


    	/** Greater than or equal to relational operator
	 */
	bool	    	    operator>=(const SocketType &) const;

        
        /**
         */
        static const string &      name(int);
        

    	/** Write SocketType to an ostream.
	 */
	ostream &           writeTo(ostream &) const;
	

    private:

        static const string     tcp;
        static const string     udp;
        static const string     unknown;
            	
    	int  	    	    type_;
};


} // namespace Transport
} // namespace Vovida


#endif // !defined(SOCKET_TYPE_DOT_H)
