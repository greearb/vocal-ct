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



#include <string.h>


#include "FileDescriptor.hxx"
#include "VLog.hxx"
#include "SystemException.hxx"
#include "SystemStatus.hxx"
#include "VocalCommon.hxx"
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

using Vocal::IO::FileDescriptor;
using Vocal::IO::file_descriptor_t;
using Vocal::Logging::VLog;
using Vocal::SUCCESS;


const file_descriptor_t	    FileDescriptor::INVALID = -1;


FileDescriptor::FileDescriptor(file_descriptor_t fd)
    :	fd_(fd)
{
}


FileDescriptor::~FileDescriptor() throw (Vocal::SystemException)
{
    if ( fd_ != INVALID )
    {
        try
        {
    	    close();
        }
        catch ( ... )
        {
        }
    }
}


void	
FileDescriptor::setFD(file_descriptor_t fd)
{
    if ( fd_ != INVALID )
    {
    	close();
    }
    fd_ = fd;
}


file_descriptor_t   	
FileDescriptor::getFD() const
{
    return ( fd_ );
}


void  	    	
FileDescriptor::setBlocking()
throw ( Vocal::SystemException )
{
    const string    fn("FileDescriptor::setBlocking");
    VLog    	    log(fn);

#if defined(WIN32)
    VERR(log) << "not implemented under Win32" << VERR_END(log);
    assert(0);
#else
    
    int flags = fcntl(fd_, F_GETFL);
    
    if	(   flags < 0 
    	||  fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK) <  0
    	)
    {
    	throw Vocal::SystemException(fn + " on fcntl(): " + strerror(errno), 
	    	    	    	__FILE__, __LINE__, errno);
    }

    VDEBUG(log) << (fn + ": fd = ") << fd_ << VDEBUG_END(log);
#endif
}


void  	    	
FileDescriptor::setNonblocking()
throw ( Vocal::SystemException )
{
    const string    fn("FileDescriptor::setNonblocking");
    VLog    	    log(fn);

#if defined(WIN32)
    VERR(log) << "not implemented under Win32" << VERR_END(log);
    assert(0);
#else
    int flags = fcntl(fd_, F_GETFL);
    
    if  (   flags < 0 
    	||  fcntl(fd_, F_SETFL, flags | O_NONBLOCK) < 0
    	)
    {
    	throw Vocal::SystemException(fn + " on fcntl(): " + strerror(errno), 
	    	    	    	__FILE__, __LINE__, errno);
    }

    VDEBUG(log) << (fn + ": fd = ") << fd_ << VDEBUG_END(log);
#endif
}


int 	
FileDescriptor::write(const string & stuff)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( stuff.size() == 0 )
    {
    	return ( 0 );
    }
    
    void * s = reinterpret_cast<void *>(const_cast<char *>(stuff.c_str()));
    
    return ( writeToFD(s, stuff.size()) );
}


int 	
FileDescriptor::write(const char * stuff)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    size_t  size = ( stuff ? strlen(stuff) : 0 );
    
    if ( size == 0 )
    {
    	return ( 0 );
    }

    void * s = reinterpret_cast<void *>(const_cast<char *>(stuff));
    
    return ( writeToFD(s, size) );
}


int 	
FileDescriptor::write(const vector<u_int8_t> & stuff)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( stuff.size() == 0 )
    {
    	return ( 0 );
    }

    void * s = reinterpret_cast<void *>(const_cast<u_int8_t *>(&stuff[0]));

    return ( writeToFD(s, stuff.size()) );
}


int 	
FileDescriptor::write(const u_int8_t * stuff, size_t size)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( stuff == 0 || size == 0 )
    {
    	return ( 0 );
    }

    void * s = reinterpret_cast<void *>(const_cast<u_int8_t *>(stuff));

    return ( writeToFD(s, size) );
}


int 	
FileDescriptor::read(string & stuff)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    void * s = reinterpret_cast<void *>(const_cast<char *>(stuff.data()));

    int bytesRead = readFromFD(s, stuff.size());

    stuff.resize(bytesRead);
        
    return ( bytesRead );
}


int 	
FileDescriptor::read(char * stuff, size_t capacity)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( stuff == 0 || capacity == 0 )
    {
    	return ( 0 );
    }
    
    void * s = reinterpret_cast<void *>(stuff);

    int bytesRead = readFromFD(s, (capacity > 1 ? capacity-1 : 1 ) );

    if ( capacity > 1 )
    {    
    	stuff[bytesRead] = '\0';
    }

    return ( bytesRead );
}


int 	
FileDescriptor::read(vector<u_int8_t> & stuff)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    stuff.clear();
    
    void * s = reinterpret_cast<void *>(&stuff[0]);

    int bytesRead = readFromFD(s, stuff.size());

    stuff.resize(bytesRead);
    
    return ( bytesRead );
}


int 	
FileDescriptor::read(u_int8_t * stuff, size_t capacity) 
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    if ( stuff == 0 || capacity == 0 )
    {
    	return ( 0 );
    }
    
    void * s = reinterpret_cast<void *>(stuff);

    int bytesRead = readFromFD(s, capacity);
    
    return ( bytesRead );
}


void  	    	
FileDescriptor::close()
throw ( Vocal::SystemException )
{
    const string    fn("FileDescriptor::close");
    VLog    	    log(fn);

    #ifndef WIN32
    if ( ::close(fd_) < 0 )
    #else
    if ( ::closesocket(fd_) == SOCKET_ERROR )
    #endif
    {
    	fd_ = INVALID;
    	throw Vocal::SystemException(fn + " on std::close(): " + strerror(errno), 
	    	    	    	__FILE__, __LINE__, errno);
    }

    VDEBUG(log) << (fn + ": fd = ") << fd_ << VDEBUG_END(log);

    fd_ = INVALID;
}


bool	    	    
FileDescriptor::operator==(const FileDescriptor & rhs) const
{
    return ( fd_ == rhs.fd_ );
}


bool	    	    
FileDescriptor::operator!=(const FileDescriptor & rhs) const
{
    return ( fd_ != rhs.fd_ );
}


bool	    	    
FileDescriptor::operator<(const FileDescriptor & rhs) const
{
    return ( fd_ < rhs.fd_ );
}


bool	    	    
FileDescriptor::operator<=(const FileDescriptor & rhs) const
{
    return ( fd_ <= rhs.fd_ );
}


bool	    	    
FileDescriptor::operator>(const FileDescriptor & rhs) const
{
    return ( fd_ > rhs.fd_ );
}


bool	    	    
FileDescriptor::operator>=(const FileDescriptor & rhs) const
{
    return ( fd_ >= rhs.fd_ );
}


ostream &       
FileDescriptor::writeTo(ostream & out) const
{
    return ( out << "fd = " << fd_ );
}


int 	
FileDescriptor::writeToFD(void * stuff, int size)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    const string    fn("FileDescriptor::writeToFD");
    VLog    	    log(fn);

    int bytesWritten = 0;
 
#ifndef WIN32
    bytesWritten = ::write( fd_, 
	    	    	    stuff, 
			    size);

    if ( bytesWritten < SUCCESS )
    {
    	int error = errno;
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without sending the packet. If the socket is set 
	// blocking we can get an EINTR here, without sending the packet.
	// 
	if ( error == EAGAIN || error == EINTR )
	{
    	    throw Vocal::SystemStatus(fn + " on write(): " + strerror(error),
	    	    	    	__FILE__, __LINE__, error);
    	}	

    	throw Vocal::SystemException(fn + " on write(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }

    VDEBUG(log) << fn << ": written on " << *this
		<< ", bytes written: " << bytesWritten
		<< ", of total: " << size
		<< VDEBUG_END(log);
#else
    // TODO
    assert(0);
#endif

    return ( bytesWritten );
}
				

int 	
FileDescriptor::readFromFD(void * stuff, int capacity)
throw ( Vocal::SystemException, Vocal::SystemStatus )
{
    const string    fn("FileDescriptor::readFromFD");
    VLog    	    log(fn);

    int bytesRead = 0;

#ifndef WIN32
    bytesRead = ::read( fd_, 
	    	    	stuff, 
			capacity);

    if ( bytesRead < SUCCESS )
    {
    	int error = errno;
	
    	// If the socket is set nonblocking, we can get an EAGAIN
	// here, without receiving the packet. If the socket is set 
	// blocking we can get an EINTR here, without receiving the packet.
	// 
	if ( error == EAGAIN || error == EINTR )
	{
    	    throw Vocal::SystemStatus(fn + " on read(): " + strerror(error),
	    	    	    	__FILE__, __LINE__, error);
    	}	

    	throw Vocal::SystemException(fn + " on read(): " + strerror(error), 
	    	    	    	__FILE__, __LINE__, error);
    }

    VDEBUG(log) << fn << ": read on " << *this
		<< ", bytes read: " << bytesRead
		<< ", of capacity: " << capacity
		<< VDEBUG_END(log);
#else
    // TODO
    assert(0);
#endif

    return ( bytesRead );
}


