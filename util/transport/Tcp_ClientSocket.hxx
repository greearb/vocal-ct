#ifndef TcpClientSocket_hxx
#define TcpClientSocket_hxx

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


//User define class
#include "Connection.hxx"
#include "VNetworkException.hxx"
#include "Data.hxx"
#include <Sptr.hxx>


class NetworkAddress;

/**
   Create A client socket for TCP.<p>

   <b>Usage:</b>

   <pre>
   NetworkAddress serverAddress;
   TcpClientSocket mySocket(serverAddress, true, false);  // nonblocking

   mySocket.connect(); // connect

   Connection& conn = mySocket.getConn();

   // now, you can use connection to read or write to the server.  see
   // Connection class for details.

   </pre>
*/
class TcpClientSocket: public BugCatcher {
public:

   /**
      Create client TCP connection.
      
      @param hostName   host:port to connect to.  Host can be fqdn or ip addr as string.
      @param closeCon   allow close() to close the connection.
      @param blocking   reads should be blocking.
      @param local_dev_to_bind_to  If specified, we'll bind tightly to this interface.
      @param local_ip_to_bind_to  If specified, we'll use this for a source IP.
   */
   TcpClientSocket(uint16 tos, uint32 priority,
                   const string& hostName,
                   const string& local_dev_to_bind_to,
                   const string& local_ip_to_bind_to,
                   bool closeCon, bool blocking);

   /**
      Create client TCP connection.
      
      @param hostName   host name to connect to.  Host can be fqdn or ip addr as string.
      @param port       host port to connect to.
      @param closeCon   allow close() to close the connection.
      @param blocking   reads should be blocking.
      @param local_dev_to_bind_to  If specified, we'll bind tightly to this interface.
      @param local_ip_to_bind_to  If specified, we'll use this for a source IP.
      
   */
   TcpClientSocket(uint16 tos, uint32 priority,
                   const string& hostName, int servPort,
                   const string& local_dev_to_bind_to,
                   const string& local_ip_to_bind_to,
                   bool closeCon, bool blocking);
   
   /**
      Create client TCP connection.
      
      @param server    host and port to connect to.
      @param closeCon   allow close() to close the connection.
      @param blocking   reads should be blocking.
      @param local_dev_to_bind_to  If specified, we'll bind tightly to this interface.
      @param local_ip_to_bind_to  If specified, we'll use this for a source IP.
      
   */
   TcpClientSocket(uint16 tos, uint32 priority,
                   const NetworkAddress& server,
                   const string& local_dev_to_bind_to,
                   const string& local_ip_to_bind_to,
                   bool closeCon, bool blocking);
   
   virtual ~TcpClientSocket();


   /**
      connect to the far side.
   */
   void connect();// throw (VNetworkException&);
   
   /**
      close the connnection.
   */
   void close();

   /**
      get the Connection which was created by this object.
   */
   Sptr <Connection> getConn() {
      return _conn;
   };

   void setConnection(Sptr<Connection> c) {
      _conn = c;
   }

   virtual bool isConnected() const;
   virtual bool isConnectInProgress() const;

   virtual bool isLive() {
      return (isConnected() || isConnectInProgress());
   }
   
   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);
   
   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);
   
   virtual void clear();

private:
   void initalize();
   //string connectionDesc(struct addrinfo* laddr) const;
   
   string local_dev_to_bind_to;
   string local_ip_to_bind_to;
   
   Sptr <Connection> _conn;
   Data _hostName;
   int _serverPort;
   bool _closeCon;
   bool _blocking;
   uint16 _tos;
   uint32 _skb_priority;
   
   // These are not implemented (and should not be)
   TcpClientSocket(const TcpClientSocket&);
   TcpClientSocket& operator=(TcpClientSocket& other);
};

#endif
