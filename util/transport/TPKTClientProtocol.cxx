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


static const char* const TPKTClientProtocol_cxx_Version = 
    "$Id: TPKTClientProtocol.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "global.h"
#include "TPKTClientProtocol.hxx"
#include "TPKTClientSocket.hxx"
#include "SystemStatus.hxx"
#include "VLog.hxx"
#include <cerrno>


using Vocal::Transport::TPKTClientProtocol;
using Vocal::Transport::Protocol;
using Vocal::Transport::TPKTClientSocket;
using Vocal::Logging::VLog;


TPKTClientProtocol::TPKTClientProtocol(const char * name)
    :	Protocol(name ? name : "TPKTClientProtocol"),
    	pkt_(0),
    	pktPosition_(0),
	pktLength_(0)
{
    awaitIncoming(true);
}


TPKTClientProtocol::~TPKTClientProtocol()
{
}


void	    	    	    
TPKTClientProtocol::onIncomingAvailable()
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    const string    fn("TPKTClientProtocol::onIncomingAvailable");
    VLog    	    log(fn);
    
    // If we are at the start of a new packet, read the header.
    // If we don't receive the whole header, a status of EAGAIN will be thrown
    // and it will be the user's responsibility to call it again later,
    // most likely after selecting on the file descriptor again.
    //
    if ( pkt_ == 0 )
    {
    	pktLength_ = getTPKTClientSocket().receiveTPKTHeader();
        pkt_ = new vector<u_int8_t>(pktLength_);
	pktPosition_ = 0;
    }

    // Get the current position of the array and start adding there.
    // If we don't receive the whole packet, throw a status of EAGAIN
    // and it will be the user's responsibility to call it again later,
    // most likely after selecting  on the file descriptor again.
    //
    u_int8_t	*   bytes   	= &((*pkt_)[pktPosition_]);
    int     	    bytesLeft 	= pktLength_ - pktPosition_;

    pktPosition_ += getTPKTClientSocket().receive(bytes, bytesLeft);

    if ( pktPosition_ < pktLength_ )
    {
    	throw Vocal::SystemStatus(fn + " parital TPKT header received.",
	    	    	    	__FILE__, __LINE__, EAGAIN);
    }

    // We have a packet, so notify the user.
    //

    VDEBUG(log) << fn << ": TPKT arrived: length = " << pktLength_ 
    	    	<< VDEBUG_END(log);

    Sptr< vector<u_int8_t> >	newPkt = pkt_;

    pkt_    	    = 0;
    pktPosition_    = 0;
    pktLength_      = 0;

    // This may cause the object to be deleted. Don't put anything
    // after onTPKTArrived.
    //    		
    onTPKTArrived(*newPkt);
}
