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


static const char* const Socket_cxx_Version = 
    "$Id: Socket.cxx,v 1.2 2005/03/03 19:59:50 greear Exp $";


#include "global.h"
#include "Socket.hxx"
#include "TransportCommon.hxx"
#include "SystemException.hxx"
#include "TransportAddress.hxx"
#include "SocketType.hxx"
#include "VLog.hxx"
#include <cerrno>
#include <sys/types.h>
#include "misc.hxx"


using Vocal::IO::file_descriptor_t;
using Vocal::IO::FileDescriptor;
using Vocal::Transport::Socket;
using Vocal::Transport::TransportAddress;
using Vocal::Transport::AddressFamily;
using Vocal::Transport::SocketType;
using Vocal::Logging::VLog;


Socket::Socket(uint16 tos, uint32 priority,
               const AddressFamily& addressFamily,
               const SocketType& socketType, const char* name,
               file_descriptor_t fd)
throw ( Vocal::SystemException )
    :	FileDescriptor(fd),
    	localAddr_(TransportAddress::create(addressFamily())),
    	addressFamily_(addressFamily),
    	socketType_(socketType),
	totalBytesSent_(0),
	totalBytesReceived_(0),
	name_(name ? name : "")
{
    const string    fn("Socket::Socket");
    VLog    	    log(fn);

    // Finish the name.
    //
    name_ += "Socket";
        
    // File descriptor may have been passed in already.
    // If so, assume it has already been bound.
    //
    if (fd_ == INVALID) {
       fd_ = socket(addressFamily(), socketType_(), 0);
    
       if (fd_ <= INVALID ) {
          fd_ = INVALID;
          throw Vocal::SystemException(fn + " on socket(): " + strerror(errno), 
                                       __FILE__, __LINE__, errno);
       }
    
       // Bind the socket.
       //
       if (bind(fd_, localAddr_->getAddress(), 
                localAddr_->getAddressLength()) < SUCCESS) {
          close(); 
          throw Vocal::SystemException(fn + " on bind(): " + strerror(errno), 
                                       __FILE__, __LINE__, errno);
       }

       // Set TOS & Priority
       vsetPriorityHelper(fd_, priority);
       vsetTosHelper(fd_, tos);
    }
        
    // Update the transport address. Don't catch the exception, let it pass.
    //
    localAddr_->updateAddress(*this);

    // Report back, for the curious among us.
    //
    VDEBUG(log) << fn << ": "  << *this << VDEBUG_END(log);
}


Socket::Socket(uint16 tos, uint32 priority,
               const TransportAddress  &   localAddr,
               const SocketType 	    &   socketType,
               const char	    	    *	name
   )   	
   throw ( Vocal::SystemException )
      :	localAddr_(localAddr.clone()),
    	addressFamily_(localAddr.getAddressFamily()),
    	socketType_(socketType),
	totalBytesSent_(0),
	totalBytesReceived_(0),
	name_(name ? name : "")
{
    const string    fn("Socket::Socket");
    VLog    	    log(fn);
    
    // Finish the name.
    //
    name_ += "Socket";
        
    // Create the socket.
    //
    AddressFamily   	addressType = localAddr_->getAddressFamily();

    fd_ = socket(addressType(), socketType_(), 0);
    
    if ( fd_ <= INVALID ) {
       fd_ = INVALID;
       throw Vocal::SystemException(fn + " on socket(): " + strerror(errno), 
                                    __FILE__, __LINE__, errno);
    }
    
    // Bind the socket.
    //
    if (bind(fd_, localAddr_->getAddress(), localAddr_->getAddressLength()) < SUCCESS) {
       close();
       VWARN(log)  << fn << ": Could not bind to address: " << *localAddr_
                   << VWARN_END(log);
       
       throw Vocal::SystemException(fn + " on bind(): " + strerror(errno), 
                                    __FILE__, __LINE__, errno);
    }
    
    // Set TOS & Priority
    vsetPriorityHelper(fd_, priority);
    vsetTosHelper(fd_, tos);

    // Update the transport address. Don't catch the exception, let it pass.
    //
    localAddr_->updateAddress(*this);
    
    // Report back, for the curious among us.
    //
    VDEBUG(log) << fn << ": "  << *this << VDEBUG_END(log);
}
    

Socket::~Socket() {
   VLog    log("Socket::~Socket");
   
   delete localAddr_;
}


const 	TransportAddress &
Socket::getLocalAddress() const {
   return ( *localAddr_ );
}


const 	AddressFamily &
Socket::getAddressFamily() const {
   return ( addressFamily_ );
}


const	SocketType &	    	
Socket::getSocketType() const {
   return ( socketType_ );
}
    

unsigned long	    	
Socket::bytesSent() const {
   return ( totalBytesSent_ );
}


unsigned long	    	
Socket::bytesReceived() const {
   return ( totalBytesReceived_ );
}


ostream &       
Socket::writeTo(ostream & out) const {
   out << name_ 
       << ": protocol = "	<< addressFamily_
       << ", type = "  	<< socketType_
       << ", ";
   
   FileDescriptor::writeTo(out);
   
   out << ", local = " 	<< *localAddr_ 
       << ", sent = "  	<< totalBytesSent_
       << ", received = "	<< totalBytesReceived_;
   
   return ( out );
}
