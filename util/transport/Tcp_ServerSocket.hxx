#ifndef TcpServerSocket_hxx
#define TcpServerSocket_hxx

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

#ifndef __MINGW32__
#include <sys/socket.h>
#include <netinet/in.h>
#endif

//User define class
#include "mstring.hxx"
#include "Connection.hxx"
#include <BugCatcher.hxx>
#include <Sptr.hxx>

class TcpServerSocket;


/**
   Create a TCP server socket.<P>

   <b>Usage:</b>


   <pre>

   TcpServerSocket serverSocket(8000); // listen on port 8000

   while (serverSocket.getServerConn().isReadReady())
   {
       Connection conn;
       serverSocket.accept(conn);
       
       // now conn can be read / written (see Connection for details)
       // to get the actual data.

   }
   </pre>

*/

class TcpServerSocket: public BugCatcher {
public:

   /**
      Create a TCP server socket.
      
      @param local_ip   Bind to this local IP if specified.
      @param servPort   listen on this port.
      * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
   */
   TcpServerSocket(uint16 tos, uint32 priority,
                   const string& local_ip,
                   const string& local_dev_to_bind_to,
                   int servPort, bool blocking);
   
   virtual ~TcpServerSocket() throw (Vocal::SystemException);

   /**
      Accept an incoming connection, and create a Connection
      object for it.
      
      @param con   the client connection is set to this object.
   */
   int accept(Connection& con);

   /** Returns < 0 if cannot immediately accept a connection, uses
    * select to determine if can select or not.
    */
   int acceptNB(Connection& con);

   /**
      close the server connection.
   */
   void close();

   /// get the server connection
   Sptr <Connection> getServerConn() {
      return _serverConn;
   };

   /// Get the file descriptor for the socket - use with care or not at all
   int getSocketFD () { return _serverConn->getConnId(); }

   /// Add this stacks file descriptors to the the fdSet
   void addToFdSet ( fd_set* set );

   /// Find the max of any file descripts in this stack and the passed value
   int getMaxFD ( int prevMax);

   /// Check and see if this stacks file descriptor is set in fd_set
   bool checkIfSet ( fd_set* set );

   const string& getSpecifiedLocalIp() const { return local_ip; }

protected:
   string local_ip; // May be "", desired local IP address, or "" for default.
   string curLocalIp; // What we really bound to (if we did)
   uint16 _tos;
   uint32 _skb_priority;

private:
   ///
   int listenOn(const string& local_ip, const string& local_dev_to_bind_to,
                int servPort);

   Sptr<Connection> _serverConn;

   // Don't be using these:  Copying file descriptors leads to too many
   // easy ways to create errors. --Ben
   TcpServerSocket(const TcpServerSocket&);   
   TcpServerSocket& operator=(TcpServerSocket& other);

};

#endif
