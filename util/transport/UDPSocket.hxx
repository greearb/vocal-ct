#if !defined(VOCAL_UDP_SOCKET_HXX)
#define VOCAL_UDP_SOCKET_HXX

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


#include "DatagramSocket.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class IPAddress;


/** UDP socket is a connection-oriented or connectionless
 *  abstraction for unreliable message transport, using the
 *  UDP/IP protocol.<br><br>
 *
 *  @see    Vocal::Transport::DatagramSocket
 *  @see    Vocal::Transport::Socket
 *  @see    Vocal::Transport::IPAddress
 *  @see    Vocal::Transport::TransportAddress
 *  @see    Vocal::SystemException
 */
class UDPSocket : public Vocal::Transport::DatagramSocket {
   
public:

   /** Default construct with a optional name.
    */
   UDPSocket(uint16 tos, uint32 priority, const char * name = 0)
      throw ( Vocal::SystemException );


   /** Construct, binding to the given local address and
    *  with a optional name.
    */
   UDPSocket(uint16 tos, uint32 priority,
             IPAddress& localAddr, 
             const char* name = 0)
      throw ( Vocal::SystemException );


   /** Construct, binding to the given local address,
    *  connecting to the remote address and with a optional name.
    */
   UDPSocket(uint16 tos, uint32 priority,
             IPAddress& localAddr, IPAddress& remoteAddr,
             const char* name = 0)
      throw ( Vocal::SystemException );
    					
   /** If connected, this will return a pointer to the remote ip address.
    *  Otherwise 0 will be returned.
    */
   Sptr<IPAddress> getRemoteIPAddress() const;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_UDP_SOCKET_HXX)
