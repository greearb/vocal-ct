#if !defined(VOCAL_TPKT_CLIENT_PROTOCOL_HXX)
#define VOCAL_TPKT_CLIENT_PROTOCOL_HXX

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


static const char* const TPKTClientProtocol_hxx_Version = 
    "$Id: TPKTClientProtocol.hxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "TransportCommon.hxx"
#include "Protocol.hxx"
#include "Sptr.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "ConnectionBrokenException.hxx"
#include <vector>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class TPKTClientSocket;


/** The TPKTClientProtocol will receive bytes from the wire and
 *  notify the user, via the onTPKTArrived, when a complete TPKT
 *  has been received, even if multiple TCP packets were sent.<br><br>
 *
 *  @see    Vocal::Transport::Protocol
 *  @see    Vocal::Transport::TPKTClientSocket
 *  @see    Vocal::SystemException
 *  @see    Vocal::SystemStatus
 *  @see    Vocal::ConnectionBrokenException
 */
class TPKTClientProtocol : public Vocal::Transport::Protocol
{
    public:


	/** Create with an optional name
	 */
    	TPKTClientProtocol(const char * name = 0);


	/** Virtual destructor
	 */
    	virtual ~TPKTClientProtocol();


	/** Get the TPKTClientSocket associated with this protocol
	 */
    	virtual TPKTClientSocket &     	    
	    	getTPKTClientSocket() = 0;
		

	/** Abstract callback that notifies the user of the 
	 *  availability of a complete TPKT.
	 */
    	virtual void	    	    	    
	    	onTPKTArrived(vector<u_int8_t> & message) = 0;


    	/** Override's Protocol's onIncomingAvailable. This will read
	 *  the bytes from the file descriptor until a complete TPKT
	 *  is available, at which point it will call onTPKTArrived.
	 */
	virtual void	    	    	    
	    	onIncomingAvailable()
		throw ( Vocal::SystemException, 
		    	Vocal::SystemStatus, 
			Vocal::Transport::ConnectionBrokenException );

    private:
    	    	Sptr< vector<u_int8_t> >    pkt_;
		u_int16_t   	    	    pktPosition_;
		u_int16_t   	    	    pktLength_;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_TPKT_CLIENT_PROTOCOL_HXX)
