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

#if !defined(WIN32)

#include "global.h"
#include "Pipe.hxx"
#include "VLog.hxx"
#include "SystemException.hxx"
#include "VocalCommon.hxx"
#include <string>
#include <unistd.h>
#include <cerrno>


using Vocal::IO::Pipe;
using Vocal::IO::FileDescriptor;
using Vocal::IO::file_descriptor_t;
using Vocal::Logging::VLog;
using Vocal::ReturnCode;
using Vocal::SUCCESS;


Pipe::Pipe(
    const file_descriptor_t 	*   descriptors
)
throw ( Vocal::SystemException )
    :	readFD_(FileDescriptor::INVALID),
    	writeFD_(FileDescriptor::INVALID)
{
    const string    fn("Pipe::Pipe");
    VLog    	    log(fn);
    
    VVERBOSE(log) << fn << ": this = " << this << VVERBOSE_END(log);
    
    if ( descriptors != 0 )
    {
    	readFD_.setFD(descriptors[0]);
	writeFD_.setFD(descriptors[1]);
    }
    else
    {
    	int filedes[2];
	filedes[0] = filedes[1] = FileDescriptor::INVALID;

    	ReturnCode  rc = ::pipe(filedes);
		
	if ( rc != SUCCESS )
	{
	    throw Vocal::SystemException(fn + "on pipe(): " + strerror(errno),
	    	    	    __FILE__, __LINE__, errno);
	}
	
	readFD_.setFD(filedes[0]);
	writeFD_.setFD(filedes[1]);
    }

    VDEBUG(log) << fn << ": " << *this << VDEBUG_END(log);
}


Pipe::Pipe(
    const FileDescriptor    & 	readFd,
    const FileDescriptor    &	writeFd
)
    :	readFD_(FileDescriptor::INVALID),
    	writeFD_(FileDescriptor::INVALID)
{
    const string    fn("Pipe::Pipe");
    VLog    	    log(fn);
    
    VVERBOSE(log) << fn << ": this = " << this << VVERBOSE_END(log);
    
    readFD_.setFD(readFd.getFD());
    writeFD_.setFD(writeFd.getFD());

    VDEBUG(log) << fn << ": " << *this << VDEBUG_END(log);
}	

Pipe::~Pipe()
throw ( Vocal::SystemException )
{
    const string    fn("Pipe::~Pipe");
    VLog    	    log(fn);
    
    VVERBOSE(log) << fn << ": this = " << this << VVERBOSE_END(log);
}    


FileDescriptor &    
Pipe::operator[](size_t index)
{
    if ( index == 0 )
    {
    	return ( readFD() );
    }
    
    return ( writeFD() );
}
	

FileDescriptor &    
Pipe::readFD()
{
    return ( readFD_ );
}


const FileDescriptor &    
Pipe::readFD() const
{
    return ( readFD_ );
}


FileDescriptor &    
Pipe::writeFD()
{
    return ( writeFD_ );
}


const FileDescriptor &    
Pipe::writeFD() const
{
    return ( writeFD_ );
}


bool	
Pipe::operator==(const Pipe & src) const
{
    return ( readFD_ == src.readFD_ && writeFD_ == src.writeFD_ );
}


bool	
Pipe::operator<(const Pipe & src) const
{
    return  ( 	readFD_ < src.readFD_ 
    	    ||	(   readFD_ == src.readFD_ && writeFD_ < src.writeFD_ )
	    );
}


ostream &       
Pipe::writeTo(ostream & out) const
{
    return  ( 	out <<"pipe = { read_" << readFD_
	    	    << ", write_" << writeFD_ << " }"
	    );
}
#endif
