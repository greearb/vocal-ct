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


static const char* const TPKTClientSocket_cxx_Version = 
    "$Id: TPKTClientSocket.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "global.h"
#include "TPKTClientSocket.hxx"
#include "IPAddress.hxx"
#include "TransportCommon.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "VLog.hxx"
#include <cerrno>


using Vocal::Transport::TPKTClientSocket;
using Vocal::Transport::TCPClientSocket;
using Vocal::Transport::IPAddress;
using Vocal::IO::file_descriptor_t;
using Vocal::Logging::VLog;


const u_int8_t      TPKTClientSocket::TPKT_FLAG_[2] = { 0x03, 0x00 };


TPKTClientSocket::TPKTClientSocket(
    const char      	*   name
)
throw ( Vocal::SystemException )
    :	TCPClientSocket(name ? name : "TPKTClient"),
    	headerPosition_(0)
{
    header_[0] = header_[1] = header_[2] = header_[3] = 0;
}


TPKTClientSocket::TPKTClientSocket(
    const IPAddress 	&   localAddr,
    const char      	*   name
)
throw ( Vocal::SystemException )
    : 	TCPClientSocket(localAddr, (name ? name : "TPKTClient")),
    	headerPosition_(0)
{
    header_[0] = header_[1] = header_[2] = header_[3] = 0;
}


TPKTClientSocket::TPKTClientSocket(
    const IPAddress 	&   localAddr,
    const IPAddress 	&   remoteAddr,
    const char      	*   name
)
throw ( Vocal::SystemException, Vocal::SystemStatus )
    : 	TCPClientSocket(localAddr, remoteAddr, (name ? name : "TPKTClient")),
    	headerPosition_(0)
{
    header_[0] = header_[1] = header_[2] = header_[3] = 0;
}


TPKTClientSocket::TPKTClientSocket(
    file_descriptor_t 	    fd,
    const IPAddress 	&   remoteAddr,
    const char      	*   name
)
throw ( Vocal::SystemException, Vocal::SystemStatus )
    : 	TCPClientSocket(fd, remoteAddr, (name ? name : "TPKTClient")),
    	headerPosition_(0)
{
    header_[0] = header_[1] = header_[2] = header_[3] = 0;
}


TPKTClientSocket::~TPKTClientSocket()
{
}


int 	
TPKTClientSocket::send(const string & message)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    sendTPKTHeader(message.size());
    
    return ( StreamClientSocket::send(message) );
}


int 	
TPKTClientSocket::send(const char * message)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    int messageSize = ( message ? strlen(message) : 0 );
    
    sendTPKTHeader(messageSize);
    
    return ( StreamClientSocket::send(message) );
}


int 	
TPKTClientSocket::send(const vector<u_int8_t> & message)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    sendTPKTHeader(message.size());
    
    return ( StreamClientSocket::send(message) );
}


int 	
TPKTClientSocket::send(const u_int8_t * message, size_t messageSize)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    sendTPKTHeader(messageSize);
    
    return ( StreamClientSocket::send(message, messageSize) );
}
				

u_int16_t 	
TPKTClientSocket::receiveTPKTHeader()
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    const string    fn("TPKTClientSocket::receiveTPKTHeader");
    VLog    	    log(fn);

    u_int8_t	*   bytes   	= &header_[headerPosition_];
    int     	    bytesLeft 	= 4 - headerPosition_;
    
    headerPosition_ += StreamClientSocket::receive(bytes, bytesLeft);

    if ( headerPosition_ < 4 )
    {
    	throw Vocal::SystemStatus(fn + " parital TPKT header received.",
	    	    	    	__FILE__, __LINE__, EAGAIN);
    }

    if ( header_[0] != TPKT_FLAG_[0] || header_[1] != TPKT_FLAG_[1] )
    {
    	VDEBUG(log) << fn << ": TPKT header/reserved version incorrect."
	    	   << VDEBUG_END(log);
				
    }
    
    u_int16_t	    length = ntohs(*(u_int16_t *)(&header_[2]));

    length = (length < 4 ? 0 : length - 4);

    clearTPKTHeader();
    
    VDEBUG(log) << fn << ": remaining length = " << length << VDEBUG_END(log);
    
    return ( length );
}


void	
TPKTClientSocket::clearTPKTHeader()
{
    header_[0] = header_[1] = header_[2] = header_[3] = 0;
    headerPosition_ = 0;
}
		

void	    
TPKTClientSocket::sendTPKTHeader(u_int16_t  size)
throw ( Vocal::SystemException, 
    	Vocal::SystemStatus, 
	Vocal::Transport::ConnectionBrokenException )
{
    const string    fn("TPKTClientSocket::sendTPKTHeader");
    VLog    	    log(fn);

    // Include the TPKT header in the size.
    //
    size += 4;
    
    VDEBUG(log) << "TPKT size: " << size << VDEBUG_END(log);
    
    u_int8_t	    header[4];
    u_int16_t	*   length = (u_int16_t *)&header[2];
    
    header[0] = TPKT_FLAG_[0];
    header[1] = TPKT_FLAG_[1];
    *length = htons(size);
    
    StreamClientSocket::send(header, 4);
}
