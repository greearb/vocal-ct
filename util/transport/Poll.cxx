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


static const char* const Poll_cxx_Version = 
    "$Id: Poll.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "global.h"
#include "Poll.hxx"
#include "Protocol.hxx"
#include "FileDescriptor.hxx"
#include "TransportCommon.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "ConnectionBrokenException.hxx"
#include "ProtocolException.hxx"
#include "IPAddress.hxx"
#include "Lock.hxx"
#include "VLog.hxx"
#include <cerrno>
#include <iomanip>


using Vocal::Transport::Poll;
using Vocal::Transport::Protocol;
using Vocal::Transport::pollfdLess;
using Vocal::Transport::ConnectionBrokenException;
using Vocal::IO::FileDescriptor;
using Vocal::IO::file_descriptor_t;
using Vocal::IO::Pipe;
using Vocal::Threads::Lock;
using Vocal::Logging::VLog;
using Vocal::SystemException;
using Vocal::ReturnCode;
using Vocal::SUCCESS;


Poll::EventType    Poll::Incoming = 0x01;
Poll::EventType    Poll::Outgoing = 0x02;
Poll::EventType    Poll::Priority = 0x04;

#if defined(WIN32) || defined(__APPLE__)
// TODO
int poll(pollfd *, unsigned int, int)
{
    assert(0);
    return 0;
}
#endif

Poll::Poll()
    :	myProtocolsChanged(false),
	myInterruptCount(0)
{
    myInterruptor.writeFD().setBlocking();
    myInterruptor.readFD().setNonblocking();
    
    // Add interruptor to poll file descriptor list. We are marking the
    // protocols and unchanged, so that they won't get reloaded.
    //
    addInterruptorToFds();
}


Poll::~Poll()
{
}


void	    	    
Poll::registerFD(
    const FileDescriptor &      fd, 
    EventType                   flag,
    Protocol                *   protocol
)
{
    const string    fn("Poll::registerFD");
    VLog    	    log(fn);

    pollfd  	    newPollFd;
    
    newPollFd.fd = fd.getFD();
    newPollFd.events = newPollFd.revents = 0;
    
    string  	eventNames;
    bool    	awaitingSomething = false;
    
    if ( flag & Incoming )
    {
    	newPollFd.events |= POLLIN;
	eventNames += " POLLIN";
	awaitingSomething = true;
    }
    if ( flag & Outgoing )
    {
    	newPollFd.events |= POLLOUT;
	eventNames += " POLLOUT";
	awaitingSomething = true;
    }
    if ( flag & Priority )
    {
    	newPollFd.events |= POLLPRI;
	eventNames += " POLLPRI";
	awaitingSomething = true;
    }
    
    if ( !awaitingSomething )
    {
    	VDEBUG(log) << fn << ": not awaiting any notifications, unregistering."
	    	    << VDEBUG_END(log);
		    
    	unregisterFD(fd);
	return;
    }
        
    ProtocolMap::iterator   it = myProtocols.find(newPollFd);
    
    if ( it != myProtocols.end() )
    {
    	// This may be an update. If necessary, replace the pollfd, 
	// effectively reseting the events.
	//
	if ( it->first.events != newPollFd.events )
	{
	    // The whole pollfd is being used as a key in the pair, but 
	    // looking more closely, only the file descriptor is being 
	    // keyed off of. If we are here, the file descriptors 
	    // (i.e. the true keys) are the same, but the events may 
	    // be different. So casting the key portion of the pair to 
	    // a non const pollfd is ok.
	    //
	    // This is the penalty I am paying for storing data in
	    // the key.
	    //
	    pollfd & pollFd = const_cast<pollfd &>(it->first);
            pollFd = newPollFd;
	    myProtocolsChanged = true;
	}
    }
    else 
    {
    	// The is a first time registration. Add the poll file
	// descriptor / protocol pair. 
	//
    	myProtocols[newPollFd] = protocol;
	myProtocolsChanged = true;
    }

    VDEBUG(log) << fn << "\n\t" << fd
    	    	<< "\n\tprotocols changed = " << ( myProtocolsChanged ? "true" : "false" )
    	    	<< "\n\tevents: " << eventNames
    	    	<< VDEBUG_END(log);

    VVERBOSE(log) << fn << ": " << *this << VVERBOSE_END(log);
}


void	    	    
Poll::updateFD(
    const FileDescriptor    &   fd, 
    EventType                   flag,
    Protocol                *   protocol
)
{
    registerFD(fd, flag, protocol);
}


void	    	    
Poll::unregisterFD(const FileDescriptor & fd)
{
    const string    fn("Poll::unregisterFD");
    VLog    	    log(fn);

    pollfd  	pollFd;

    pollFd.fd = fd.getFD();

    bool    removed = false;
        
    ProtocolMap::iterator   it = myProtocols.find(pollFd);
    if ( it != myProtocols.end() )
    {
    	// Found it, so let's remove it.
	//
    	myProtocols.erase(it);
	myProtocolsChanged = true;
	removed = true;
    }

    VDEBUG(log) << fn << "\n\t" << fd
    	    	<< "\n\tprotocols changed = " << ( myProtocolsChanged ? "true" : "false" )
		<< "\n\tremoved = " << ( removed ? "true" : "false" )
    	    	<< VDEBUG_END(log);

    VVERBOSE(log) << fn << ": " << *this << VVERBOSE_END(log);
}


void	    	    
Poll::registerProtocol(Protocol & protocol)
{
    EventType   flag = 0;

    if ( protocol.awaitIncoming() )
    {
    	flag |= Incoming;
    }
    if ( protocol.awaitOutgoing() )
    {
    	flag |= Outgoing;
    }
    if ( protocol.awaitPriority() )
    {
    	flag |= Priority;
    }

    registerFD(protocol.getFileDescriptor(), flag, &protocol);
}


void	    	    
Poll::updateProtocol(Protocol & protocol)
{
    registerProtocol(protocol);
}


void	    	    
Poll::unregisterProtocol(Protocol & protocol)
{
    unregisterFD(protocol.getFileDescriptor());
}


int 	    	    
Poll::poll(int timeout)
throw ( Vocal::SystemException )
{
    const string    fn("Poll::poll");
    VLog    	    log(fn);
    
    // Recreate the poll file descriptor if needed.
    //
    if ( myProtocolsChanged )
    {
    	VDEBUG(log) << fn << ": protocols changed, updating." << VDEBUG_END(log);
	
    	myFds.clear();

	addInterruptorToFds();
	addProtocolsToFds();

	myProtocolsChanged = false;
    }

    VVERBOSE(log) << fn << ": " << *this << VVERBOSE_END(log);

    VDEBUG(log) << fn << ": polling for activity. timeout = " << timeout
    	    	<< VDEBUG_END(log);
    
    int numberFdsActive = ::poll(&myFds[0], myFds.size(), timeout);

    if ( numberFdsActive < SUCCESS )
    {
    	int error = errno;

    	// If we were interrupted, treat it like a timeout.
	//
    	if ( error == EINTR )
	{
	    VDEBUG(log) << fn << ": interrupted." << VDEBUG_END(log);
    	    return ( 0 );
	}
	
    	throw Vocal::SystemException(fn + " on poll(): " + strerror(error), 
	    	    	    	 __FILE__, __LINE__, error);
    }    

    // Clear interruptor so that numberFdsActive corresponds to
    // registered file descriptors only.
    //
    for (   vector<pollfd>::iterator  it = myFds.begin();
    	    it != myFds.end() && numberFdsActive > 0;
	    it++
	)
    {
    	pollfd	    &   pollFdEntry = (*it);

    	if ( processInterruptor(pollFdEntry) )
	{
    	    VVERBOSE(log) << fn << ": " << *this << VVERBOSE_END(log);
	    numberFdsActive--;
	    pollFdEntry.revents = 0;
	    break;
	}
    }
        
    return ( numberFdsActive );
}


Poll::EventType
Poll::fdActive(const FileDescriptor & filedesc) const
{
    EventType returnEvent = 0;
    
    file_descriptor_t   fd = filedesc.getFD();
    
    for (   vector<pollfd>::const_iterator i = myFds.begin();
            i != myFds.end();
            ++i
        )
    {
        if ( fd == i->fd )
        {
            returnEvent = i->revents;
            break;
        }
    }
    return ( returnEvent );
}


void 	    	    
Poll::processProtocols(int numberFdsActive)
throw ( Vocal::Transport::ProtocolException )
{
    const string    fn("Poll::processProtocols");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": Start: number file descriptors active = " 
    	    	<< numberFdsActive 
	    	<< ", protocols changed = " 
		<< ( myProtocolsChanged ? "true" : "false" )
		<< VDEBUG_END(log);

    VVERBOSE(log) << fn << ": " << *this << VVERBOSE_END(log);
		
    for (   vector<pollfd>::iterator  it = myFds.begin();
    	    it != myFds.end() && numberFdsActive > 0;
	    it++
	)
    {
    	pollfd	    &   pollFdEntry = (*it);

    	if ( pollFdEntry.fd == myInterruptor.readFD().getFD() )
	{
	    pollFdEntry.revents = 0;
	    continue;
	}

    	ProtocolMap::iterator   protocolIt = myProtocols.find(pollFdEntry);
	
	if ( protocolIt == myProtocols.end() )
	{
	    // If a two protocols are active, but then second protocol
	    // is unregistered as a result of activity on the first 
	    // protocol, we will end up here.
	    //
	    myProtocolsChanged = true;

	    VDEBUG(log) << fn << ": no protocol found for: fd = "
	    	    	<< it->fd << VDEBUG_END(log);


    	    VVERBOSE(log) << fn << ": " << *this << VVERBOSE_END(log);
	    pollFdEntry.revents = 0;
	    continue;
	}

    	Protocol    * protocol = (*protocolIt).second;
    	
        if ( protocol == 0 )
        {
            VWARN(log) << fn << ": no protocol associated with fd = " 
                       << it->fd << VWARN_END(log);
            continue;
        }
	
    	if ( processPriority(pollFdEntry, protocol) )
	{
    	    VVERBOSE(log) << fn << ": " << *this << VVERBOSE_END(log);
	    numberFdsActive--;
	    pollFdEntry.revents = 0;
	    continue;
	}
	
    	if ( processIncoming(pollFdEntry, protocol) )
	{
    	    VVERBOSE(log) << fn << ": " << *this << VVERBOSE_END(log);
	    numberFdsActive--;
	    pollFdEntry.revents = 0;
	    continue;
	}
	
    	if ( processOutgoing(pollFdEntry, protocol) )
	{
    	    VVERBOSE(log) << fn << ": " << *this << VVERBOSE_END(log);
	    numberFdsActive--;
	    pollFdEntry.revents = 0;
	    continue;
	}
	
    	if ( processHangup(pollFdEntry, protocol) )
	{
    	    VVERBOSE(log) << fn << ": " << *this << VVERBOSE_END(log);
	    numberFdsActive--;
    	    pollFdEntry.revents = 0;
	    continue;
	}
	
    	if ( processError(pollFdEntry, protocol) )
	{
    	    VVERBOSE(log) << fn << ": " << *this << VVERBOSE_END(log);
	    numberFdsActive--;
	}

    	pollFdEntry.revents = 0;
    }

    VDEBUG(log) << fn << ": End: number file descriptors active = " 
    	    	<< numberFdsActive 
	    	<< ", protocols changed = " 
		<< ( myProtocolsChanged ? "true" : "false" )
		<< VDEBUG_END(log);
}


void	    	    
Poll::interrupt()
throw ( Vocal::SystemException )
{
    const string    fn("Poll::interrupt");
    VLog    	    log(fn);
    
    Lock    lock(myInterruptorMutex); (void)lock;

    myInterruptCount++;
    
    myInterruptor.writeFD().write(&myInterruptCount, 1);

    VDEBUG(log) << fn << ": Interrupting: "   << (unsigned)myInterruptCount 
    	    	<< " outstanding interrupts." << VDEBUG_END(log);
}


u_int8_t    	    
Poll::interruptCount() const
{
    return ( myInterruptCount );
}


ostream &           
Poll::writeTo(ostream & out) const
{
    out << "protocols (size: " << myProtocols.size() << ") = \n{\n";
    for ( ProtocolMap::const_iterator it = myProtocols.begin(); it != myProtocols.end(); it++ )
    {
    	if ( it->first.fd == myInterruptor.readFD().getFD() )
	{
	    continue;
	}
	
    	out << "  ";
	if ( it->second )
	{
	    out << *it->second << "\n";
	}
	else
	{
	    out << "no protocol.\n";
	}
    }

    out << "}\ninterruptor = " << myInterruptor 
    	<< "\nnumber outstanding interruptions = " << (unsigned)myInterruptCount
	<< "\nprotocolsChanged = " << ( myProtocolsChanged ? "true" : "false" );

    size_t  size = myFds.size();
    
    out << "\nfds (size: " << size << ") = \n{\n";
    for ( size_t i = 0; i < size; i++ )
    {
    	out << " ( " << myFds[i].fd 
	    << ", 0x" << hex << setw(2) << setfill('0') << (unsigned)(myFds[i].events)
	    << ", 0x" << hex << setw(2) << setfill('0') << (unsigned)(myFds[i].revents)
	    << dec << " )\n";
    }
    out << "}";
    
    return ( out );
}
					
void	    	    
Poll::addInterruptorToFds()
{
    pollfd  	interruptorPollFd;

    interruptorPollFd.fd    	= myInterruptor.readFD().getFD();
    interruptorPollFd.events	= POLLIN;
    interruptorPollFd.revents	= 0;

    myFds.insert(myFds.end(), interruptorPollFd);
}


void	    	    
Poll::addProtocolsToFds()
{
    for (   ProtocolMap::iterator  it = myProtocols.begin();
    	    it != myProtocols.end();
	    it++
	)
    {
    	myFds.insert(myFds.end(), it->first);
    }
}


bool
Poll::processInterruptor(pollfd &   pollFdEntry)
{
    const string    fn("Poll::processInterruptor");
    VLog    	    log(fn);

    VVERBOSE(log) << fn << ": fd = " << pollFdEntry.fd << VVERBOSE_END(log);
        
    if  ( pollFdEntry.fd == myInterruptor.readFD().getFD() )
    {
    	if ( pollFdEntry.revents & POLLIN  )
	{
	    u_int8_t    interruptNumber = 0;
	    IPAddress   remoteAddr;

	    Lock    lock(myInterruptorMutex); (void)lock;

	    VDEBUG(log) << fn << ": Interrupted: " << (unsigned)myInterruptCount 
    	    		<< " outstanding interrupts." << VDEBUG_END(log);
	    do
	    {
		// Let the user catch the exception, if thrown.
		//
        	myInterruptor.readFD().read(&interruptNumber, 1);

	    } while ( interruptNumber < myInterruptCount );

	    myInterruptCount = 0;
	    
	    return ( true );
	}
	else
	{
	    return ( false );
	}
    }
    return ( false );
}


bool
Poll::processPriority(pollfd & pollFdEntry, Protocol * protocol)
{
    const string    fn("Poll::processPriority");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": fd = " << pollFdEntry.fd << VDEBUG_END(log);

    if ( pollFdEntry.revents & POLLPRI )
    {
	VDEBUG(log) << fn << ": Priority for protocol = " << *protocol
	    	    << VDEBUG_END(log);

    	// Notify the user of a priority message on the file descriptor.
	//
	try
	{
	    protocol->onPriority();
	}
	catch ( Vocal::SystemStatus & )
	{
	    // The status indicates that we need to poll again.
	    // This is the desfault behavior, so do nothing.
	}
	catch ( Vocal::Transport::ConnectionBrokenException & e)
	{
	    return ( processHangup(pollFdEntry, protocol, &e) );
	}
	catch ( Vocal::SystemException & e )
	{
	    return ( processError(pollFdEntry, protocol, &e) );
	}
	catch ( ... )
	{
	    protocol->handleUnknownException();
	}
	return ( true );
    }
    
    return ( false );
}


bool
Poll::processIncoming(pollfd & pollFdEntry, Protocol * protocol)
{
    const string    fn("Poll::processIncoming");
    VLog    	    log(fn);

    VVERBOSE(log) << fn << ": fd = " << pollFdEntry.fd << VVERBOSE_END(log);

    if ( pollFdEntry.revents & POLLIN )
    {
	VDEBUG(log) << fn << ": IncomingAvailable for protocol = " 
		    << *protocol << VDEBUG_END(log);

    	// Notify the user of an incoming message on the file 
	// descriptor.
	//
	try
	{
	    protocol->onIncomingAvailable();
	}
	catch ( Vocal::SystemStatus & )
	{
	    // The status indicates that we need to poll again.
	    // Do nothing.
	}
	catch ( Vocal::Transport::ConnectionBrokenException & e )
	{
	    return ( processHangup(pollFdEntry, protocol, &e) );
	}
	catch ( Vocal::SystemException & e )
	{
	    return ( processError(pollFdEntry, protocol, &e) );
	}
	catch ( ... )
	{
	    protocol->handleUnknownException();
	}
	return ( true );
    }
    return ( false );
}


bool
Poll::processOutgoing(pollfd & pollFdEntry, Protocol * protocol)
{
    const string    fn("Poll::processOutgoing");
    VLog    	    log(fn);

    VVERBOSE(log) << fn << ": fd = " << pollFdEntry.fd << VVERBOSE_END(log);

    if ( pollFdEntry.revents & POLLOUT )
    {
	VDEBUG(log) << fn << ": OutgoingAvailable for protocol = " 
		    << *protocol << VDEBUG_END(log);

    	// Notify the user of that the file descriptor is
	// available to send outgoing messages.
	//
	try
	{
	    protocol->onOutgoingAvailable();
	}
	catch ( Vocal::SystemStatus )
	{
	    // The status indicates that we need to poll again.
	    // Do nothing.
	}
	catch ( Vocal::Transport::ConnectionBrokenException & e )
	{
	    return ( processHangup(pollFdEntry, protocol, &e) );
	}
	catch ( Vocal::SystemException & e )
	{
	    return ( processError(pollFdEntry, protocol, &e) );
	}
	catch ( ... )
	{
	    protocol->handleUnknownException();
	}
	return ( true );
    }
    return ( false );
}


bool
Poll::processHangup(
    pollfd  	    	    	&   pollFdEntry, 
    Protocol 	    	    	*   protocol,
    ConnectionBrokenException 	*   connectionBrokenException
)
{
    const string    fn("Poll::processHangup");
    VLog    	    log(fn);

    VVERBOSE(log) << fn << ": fd = " << pollFdEntry.fd << VVERBOSE_END(log);
    
    if  (   connectionBrokenException != 0
    	||  pollFdEntry.revents & POLLHUP 
	)
    {
	VDEBUG(log) << fn << ": Disconnect for protocol = " 
		    << *protocol << VDEBUG_END(log);

    	// Notify the user of that the file descriptor has hung up.
	//
	try
	{
	    protocol->onDisconnect(connectionBrokenException);
	}
	catch ( Vocal::SystemStatus & )
	{
	    // The status indicates that we need to poll again.
	    // Do nothing.
	}
	catch ( Vocal::SystemException & e )
	{
	    return ( processError(pollFdEntry, protocol, &e) );
	}
	catch ( ... )
	{
	    protocol->handleUnknownException();
	}
	return ( true );
    }
    return ( false );
}


bool
Poll::processError(
    pollfd  	    	&   pollFdEntry, 
    Protocol 	    	*   protocol,
    SystemException	*   systemException
)
{
    const string    fn("Poll::processError");
    VLog    	    log(fn);

    VVERBOSE(log) << fn << ": fd = " << pollFdEntry.fd << VVERBOSE_END(log);

    if  (   systemException != 0 
    	||  pollFdEntry.revents & POLLERR
	||  pollFdEntry.revents & POLLNVAL
	)
    {
	VWARN(log)  << fn << ": Error for protocol = " << *protocol
	    	    << VWARN_END(log);

    	// Notify the user of the error.
	//
	try
	{
            protocol->onError(systemException);
	}
	catch ( ... )
	{
	    protocol->handleUnknownException();
	}
	return ( true );
    }
    return ( false );
}



bool 
pollfdLess::operator()(const pollfd & left, const pollfd & right) const
{
    return ( left.fd < right.fd );
}
