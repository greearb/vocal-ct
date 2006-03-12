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


#ifndef __vxworks

#include "global.h"
#include <errno.h>
#include <unistd.h>
#ifndef __MINGW32__
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif
#include <stdio.h>
#include <string.h>

#include "VNetworkException.hxx"
#include "Tcp_ClientSocket.hxx"
#include "VEnvVar.hxx"
#include "NetworkAddress.h"
#include "NetworkConfig.hxx"
#include "cpLog.h"

TcpClientSocket::TcpClientSocket(uint16 tos, uint32 priority,
                                 const string& hostName,
                                 const string& _local_dev_to_bind_to,
                                 const string& _local_ip_to_bind_to,
                                 bool closeCon, bool blocking)
    :
    local_dev_to_bind_to(_local_dev_to_bind_to),
    local_ip_to_bind_to(_local_ip_to_bind_to),
    _hostName(hostName),
    _serverPort( -1),
    _closeCon(closeCon),
    _blocking(blocking),
    _tos(tos), _skb_priority(priority)
{
    initalize();
}

TcpClientSocket::TcpClientSocket(uint16 tos, uint32 priority,
                                 const string& hostName, int servPort,
                                 const string& _local_dev_to_bind_to,
                                 const string& _local_ip_to_bind_to,
                                 bool closeCon, bool blocking)
    :
    local_dev_to_bind_to(_local_dev_to_bind_to),
    local_ip_to_bind_to(_local_ip_to_bind_to),
    _hostName(hostName),
    _serverPort(servPort),
    _closeCon(closeCon),
    _blocking(blocking),
    _tos(tos), _skb_priority(priority)
{
    initalize();
}

TcpClientSocket::TcpClientSocket(uint16 tos, uint32 priority,
                                 const NetworkAddress& server,
                                 const string& _local_dev_to_bind_to,
                                 const string& _local_ip_to_bind_to,
                                 bool closeCon, bool blocking)
    :
    local_dev_to_bind_to(_local_dev_to_bind_to),
    local_ip_to_bind_to(_local_ip_to_bind_to),
    _closeCon(closeCon),
    _blocking(blocking),
    _tos(tos), _skb_priority(priority)
{
    _hostName = server.getHostName();
    _serverPort = server.getPort();
    cpLog(LOG_DEBUG_STACK, "%s %d", _hostName.c_str(), _serverPort);
    if (_serverPort == -1 ) {
        _serverPort = VEnvVar::VPS_PORT;
    }
    initalize();
}

void TcpClientSocket::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                           uint64 now) {
    if (_conn != 0) {
        _conn->tick(input_fds, output_fds, exc_fds, now);
    }
}

int TcpClientSocket::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                            int& maxdesc, uint64& timeout, uint64 now) {
    if (_conn != 0) {
        _conn->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
    }
    return 0;
}


/* I think this is a bad idea, and it's not implemented right, as far as I
 * can tell. --Ben
TcpClientSocket::TcpClientSocket(const TcpClientSocket& other)
        : _conn(other._blocking)
{
    _conn = other._conn;
}
*/

/*
TcpClientSocket&
TcpClientSocket::operator=(TcpClientSocket& other)
{
    if (this != &other)
    {
        _conn = other._conn;
        _hostName = other._hostName;
        _serverPort = other._serverPort;
        _closeCon = other._closeCon;
        _blocking = other._blocking;
    }
    return *this;
}
*/


void TcpClientSocket::initalize() {
   _conn = new Connection(_blocking);
}

TcpClientSocket::~TcpClientSocket() {
   close();
}


bool TcpClientSocket::isConnectInProgress() const {
   return _conn->isConnectInProgress();
}

bool TcpClientSocket::isConnected() const {
    if (_conn->isLive()) {
        if (!_conn->isConnectInProgress()) {
            return true;
        }
    }
    return false;
}


void TcpClientSocket::connect() throw (VNetworkException&) {
   if (_conn->isConnectInProgress()) {
      // Already working on it.
      return;
   }

   assert(_conn->_connId < 0); // Make sure we don't leak sockets

   struct sockaddr_in name;
   uint32 tmpip;
   if (vtoIpString(_hostName.c_str(), tmpip) < 0) {
      cpLog(LOG_ERR, "ERROR:  Failed to resolve destination host: %s\n", _hostName.c_str());
      return;
   }
   name.sin_family = AF_INET;
   name.sin_port = htons(_serverPort);      
   name.sin_addr.s_addr = htonl(tmpip);

   _conn->_connId = ::socket(AF_INET, SOCK_STREAM, 0);
   if (_conn->_connId < 0) {
      char buf[256];
      sprintf(buf, "Failed to create socket: reason %s", VSTRERROR);
      cpLog(LOG_DEBUG, buf);
   }
      
   // Set it to be non-blocking, etc.
   _conn->setState();
      
#ifdef __linux__
   // Optionally, bind this to the local interface.
   if (local_dev_to_bind_to.size()) {
      // Bind to specific device.
      char dv[15 + 1];
      strncpy(dv, local_dev_to_bind_to.c_str(), 15);
      if (setsockopt(_conn->_connId, SOL_SOCKET, SO_BINDTODEVICE, dv, 15 + 1)) {
         cpLog(LOG_ERR, "ERROR:  setsockopt (BINDTODEVICE), dev: %s  error: %s\n",
               dv, VSTRERROR);
      }
   }
#endif
      
   // Bind to the local IP
   if (local_ip_to_bind_to.size()) {
      uint32 lip;
      if (vtoIpString(local_ip_to_bind_to.c_str(), lip) < 0) {
         cpLog(LOG_ERR, "ERROR:  Failed to resolve local host/ip: %s\n", local_ip_to_bind_to.c_str());
      }
      else {
         struct sockaddr_in my_ip_addr;
         memset(&my_ip_addr, 0, sizeof(my_ip_addr));
   
         my_ip_addr.sin_family = AF_INET;
         my_ip_addr.sin_addr.s_addr = htonl(lip);
         my_ip_addr.sin_port = 0;  // any local port
         if (::bind(_conn->_connId, (struct sockaddr*)(&my_ip_addr), sizeof(my_ip_addr)) != 0) {
            cpLog(LOG_ERR, "ERROR:  local bind to ip: %s failed with error: %s\n",
                  local_ip_to_bind_to.c_str(), VSTRERROR);
            // Shouldn't be fatal.
         }
      }
   }
   // Set ToS and Priority
   vsetPrio(_conn->_connId, _tos, _skb_priority, "Tcp_Client::connect");

   int rv = ::connect(_conn->_connId, (struct sockaddr*)&name, sizeof(name));
   if (rv >= 0) {
      ///Success
      cpLog(LOG_DEBUG, "Connected to %s:%i", _hostName.c_str(), _serverPort);
      delete[] _conn->_connAddr;
      _conn->_connAddr = (struct sockaddr*) new char[sizeof(name)];
      memcpy(_conn->_connAddr, &name, sizeof(name));
      _conn->_connAddrLen = sizeof(name);
   }
   else {
      int myerrno = ERRNO;
      // If we are non-blocking, then EINPROGRESS is OK
      if (myerrno == EINPROGRESS) {
         _conn->setConnectInProgress(true);
      }
      else {
         cpLog(LOG_ERR, "ERROR:  Failed to connect to %s:%i, error: %s\n",
               _hostName.c_str(), _serverPort, VSTRERROR);
      }
   }
      
   _conn->setState();
}//connect


void TcpClientSocket::clear() {
    if (_conn != 0) {
        _conn->clear();
    }
}


void TcpClientSocket::close() {
   // Flush any outstanding writes, if possible
   if (_conn != 0) {
      _conn->write();
   }
   if (_closeCon && (_conn != 0) && (_conn->getConnId() > 2)) {
      _conn->close();
   }
}
#if 0
string TcpClientSocket::connectionDesc(struct addrinfo* laddr) const {
   struct sockaddr_in* sa = (struct sockaddr_in*)(laddr);
   string ipn(vtoStringIp(ntohl(sa->sin_addr.s_addr)));
   ipn.append(":");
   ipn.append(itoa(ntohs(sa->sin_port)));
   return ipn;
}
#endif

#endif
