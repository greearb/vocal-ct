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


#include "global.h"
#include "Protocol.hxx"
#include "FileDescriptor.hxx"
#include "Poll.hxx"


using Vocal::Transport::Protocol;
using Vocal::Transport::ProtocolException;
using Vocal::Transport::ConnectionBrokenException;
using Vocal::Transport::Poll;
using Vocal::SystemException;


Protocol::Protocol(const char * pName)
    :	myIncoming(true),
    	myOutgoing(false),
    	myPriority(false),
	myName(pName ? pName : ""),
        myPoll(0)
{
}


Protocol::Protocol(Poll & poll, const char * pName)
    :   myIncoming(true),
        myOutgoing(false),
        myPriority(false),
        myName(pName ? pName : ""),
        myPoll(&poll)
{
}


Protocol::~Protocol()
{
    if ( myPoll )
    {
        myPoll->unregisterProtocol(*this);
    }
}


void	    	    	    
Protocol::onIncomingAvailable()
{
}


void	    	    	    
Protocol::onOutgoingAvailable()
{
}


void	    	    	    
Protocol::onPriority()
{
}


void	    	    	    
Protocol::onDisconnect(
    ConnectionBrokenException *
)
throw ( Vocal::Transport::ProtocolException )
{
    throw Vocal::Transport::ProtocolException(
    	    	    	    "Protocol::onDisconnect", 
		    	    ProtocolException::HANGUP_EXCEPTION,
			    *this);
}


void	    	    	    
Protocol::onError(
    SystemException *
)
throw ( Vocal::Transport::ProtocolException )
{
    throw Vocal::Transport::ProtocolException(
    	    	    	    "Protocol::onError", 
		    	    ProtocolException::ERROR_EXCEPTION,
			    *this);
}


void	    	    	    
Protocol::handleUnknownException()
{
    throw;
}

void	    	    	    
Protocol::awaitIncoming(bool incoming)
{
    myIncoming = incoming;

    if ( myPoll )
    {
        myPoll->updateProtocol(*this);
    }
}


bool	    	    	    
Protocol::awaitIncoming() const
{
    return ( myIncoming );
}


void	    	    	    
Protocol::awaitOutgoing(bool outgoing)
{
    myOutgoing = outgoing;

    if ( myPoll )
    {
        myPoll->updateProtocol(*this);
    }
}


bool	    	    	    
Protocol::awaitOutgoing() const
{
    return ( myOutgoing );
}


void	    	    	    
Protocol::awaitPriority(bool priority)
{
    myPriority = priority;

    if ( myPoll )
    {
        myPoll->updateProtocol(*this);
    }
}


bool	    	    	    
Protocol::awaitPriority() const
{
    return ( myPriority );
}


const string &
Protocol::name() const
{
    return ( myName );
}


ostream &           	    
Protocol::writeTo(ostream & out) const
{
    return  (	out << myName << ", " << getFileDescriptor()
    	    	    << ( myIncoming ? ", awaiting incoming" : "")
	    	    << ( myOutgoing ? ", awaiting outgoing" : "")
	    	    << ( myPriority ? ", awaiting priority" : "")
	    );
}


bool	    	    	    
Protocol::operator==(const Protocol & src)
{
    return ( this == &src );
}


bool	    	    	    
Protocol::operator<(const Protocol & src)
{
    return ( this < &src );
}
