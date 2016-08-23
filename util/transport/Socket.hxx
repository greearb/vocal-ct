#if !defined(SOCKET_DOT_H)
#define SOCKET_DOT_H

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


#include "TransportCommon.hxx"
#include "FileDescriptor.hxx"
#include "AddressFamily.hxx"
#include "SocketType.hxx"
#include "SystemException.hxx"
#include "misc.hxx"
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


class AddressFamily;
class TransportAddress;
class SocketType;


/** A Socket extends a FileDescriptor, specializing it with
 *  transport related properties like an address type, a socket type
 *  and a local transport address.<br><br>
 *
 *  @see    Vocal::IO::FileDescriptor
 *  @see    Vocal::Transport::AddressFamily
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::Transport::SocketType
 *  @see    Vocal::SystemException
 *  @see    Vocal::IO::file_descriptor_t
 */
class Socket : public Vocal::IO::FileDescriptor {
protected:


   /** Construct given the AddressFamily, SocketType, the optional
    *  name, and an optional already existing native file descriptor.
    *  This will create the socket and bind the socket to the next
    *  available port.
    *
    *  @exception	Vocal::SystemException
    */
   Socket(uint16 tos, uint32 priority,
          const AddressFamily& addressFamily,
          const SocketType& sockType, const char* name = 0,
          IO::file_descriptor_t fd = INVALID)
      throw ( Vocal::SystemException );


   /** Construct given the local transport address, the SocketType
    *  and the optional socket name. The socket will be created and
    *  it will be bound to the local transport address.
    *
    *  @exception	Vocal::SystemException
    */
   Socket(uint16 tos, uint32 priority,
          const TransportAddress& localAddr,
          const SocketType& sockType,
          const char* name = 0)
      throw ( Vocal::SystemException );
   
public:
    					
   /** Virtual destructor.
    */
   virtual ~Socket() throw (Vocal::SystemException);


   /** Returns the local address to which the socket is bound.
    */
   const TransportAddress& getLocalAddress() const;


   /** Returns the address family of the socket.
    */
   const AddressFamily& getAddressFamily() const;


   /** Returns the socket type.
    */
   const SocketType& getSocketType() const;    
   

   /** Returns the number of bytes sent on this socket.
    */
   unsigned long bytesSent() const;
   

   /** Returns the number of bytes received on this socket.
    */
   unsigned long bytesReceived() const;


   /** Write this Socket to an ostream.
    */
   virtual ostream& writeTo(ostream &) const;
    

protected:


   /** The local address this socket is bound to.
    */
   TransportAddress* localAddr_;


   /** The addressFamily of this socket.
    */
   AddressFamily addressFamily_;


   /** The socket type.
    */
   SocketType socketType_;


   /** The number of bytes sent.
    */
   unsigned long totalBytesSent_;


   /** The number of bytes received.
    */
   unsigned long totalBytesReceived_;


   /** The name of the socket.
    */
   string name_;

private:

   /** Copying is suppressed.
    */
   Socket(const Socket &);
	

   /** Copying is suppressed.
    */
   Socket& operator=(const Socket &);					
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(SOCKET_DOT_H)
