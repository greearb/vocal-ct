#if !defined(VOCAL_PROTOCOL_EXCEPTION_HXX)
#define VOCAL_PROTOCOL_EXCEPTION_HXX

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


static const char* const ProtocolException_hxx_Version = 
    "$Id: ProtocolException.hxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "VException.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class Protocol;


/** Thrown when the protocol is notified of exceptional events, such
 *  as the file descriptor associated with the protocol disconnecting
 *  or when an error is detected on the associated file descriptor.<br><br>
 *
 *  @see    Vocal::Transport::Protocol
 *  @see    VException
 */
class ProtocolException : public VException
{
    public:
    	
    	/** Types of exceptional conditions.
	 */
	enum Type
	{
	    NO_EXCEPTION = 0,
	    HANGUP_EXCEPTION,
	    ERROR_EXCEPTION
	};
	

    	/** constructor
	 */
        ProtocolException(  
	    const string	    & 	msg,
    	    Type    	    	    	type,
	    Protocol	    	    &	protocol);
    
    	
	/** Get class of exception name, "ProtocolException".
	 */
    	string      	    	    getName() const;


    	/** Accessor
	 */
    	Type	    	    	    getType() const;
	

	/** Access the protocol associated with the exceptional condition.
	 */
    	Protocol &  	    	    getProtocol() const;
	

    private:

    	Type	    	    	    	myType;
    	Protocol    	    	    &	myProtocol;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_PROTOCOL_EXCEPTION_HXX)
