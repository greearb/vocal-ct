
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
#endif
#include <stdio.h>
#ifndef WIN32
#include <strings.h>
#else
#include <string.h>
#endif

#include "VNetworkException.hxx"
#include "TransportCommon.hxx"
#include "Tcp_ServerSocket.hxx"
#include "VEnvVar.hxx"
#include "cpLog.h"
#include "NetworkAddress.h"

#define LISTENQ   15
TcpServerSocket::TcpServerSocket(uint16 tos, uint32 priority,
                                 const string& _local_ip,
                                 const string& local_dev_to_bind_to,
                                 int servPort, bool blocking)
{
   _serverConn = new Connection(blocking);
   local_ip = _local_ip;
   _tos = tos;
   _skb_priority = priority;
   listenOn(local_ip, local_dev_to_bind_to, servPort);
}

int TcpServerSocket::listenOn(const string& local_ip, const string& local_dev_to_bind_to,
                               int servPort) {
   struct sockaddr_in sa;
   memset(&sa, 0, sizeof(sa));
   sa.sin_family = AF_INET;
   sa.sin_port   = htons(servPort);

   uint32 ip;
   if (vtoIpString(local_ip.c_str(), ip) < 0) {
      cpLog(LOG_ERR, "ERROR:  Could not resolve local-ip: %s  error: %s\n",
            local_ip.c_str(), VSTRERROR);
      return -ERRNO;
   }

   sa.sin_addr.s_addr = htonl(ip);

   _serverConn->_connId = ::socket(AF_INET, SOCK_STREAM, 0);
   if (_serverConn->_connId < 0) {
      cpLog(LOG_ERR, "ERROR:  Failed to create socket: %s\n", VSTRERROR);
      return -ERRNO;
   }

#ifdef __linux__
   if (local_dev_to_bind_to.size()) {
      // Bind to specific device.
      char dv[15 + 1];
      memset(dv, 0, sizeof(dv));
      strncpy(dv, local_dev_to_bind_to.c_str(), 15);
      if (setsockopt(_serverConn->_connId, SOL_SOCKET, SO_BINDTODEVICE,
                     dv, 15 + 1)) {
         cpLog(LOG_ERR, "ERROR:  setsockopt (BINDTODEVICE), dev: %s  error: %s\n",
               dv, strerror(errno));
      }
   }
#endif

   int on = 1;
   if ( setsockopt ( _serverConn->_connId, SOL_SOCKET, SO_REUSEADDR, (char*)(&on), sizeof ( on )) ) {
      // this is an error -- can't set it
      cpLog(LOG_ALERT, "setsockopt failed (REUSEADDR), reason: %s", strerror(errno));
   }

   // Set ToS and Priority
   vsetPrio(_serverConn->_connId, _tos, _skb_priority, "TcpServerSocket::listenOn");

   if (::bind(_serverConn->_connId, (struct sockaddr*)(&sa), sizeof(sa))) {
      delete []_serverConn->_connAddr;
      _serverConn->_connAddr = (struct sockaddr*) new char[sizeof(sa)];
      memcpy(_serverConn->_connAddr, &sa, sizeof(sa));

      curLocalIp = vtoStringIp(ip);
         
      cpLog(LOG_INFO, "IPv4 TCP server locally bound to %s:%d",
            curLocalIp.c_str(), servPort);
   }

   if (::listen(_serverConn->_connId, LISTENQ)) {
      cpLog(LOG_ALERT, "ERROR:  listen failed, reason: %s\n", VSTRERROR);
      close();
      return -ERRNO;
   }
   return 0;
}

TcpServerSocket::~TcpServerSocket() {
    close();
}


/** Returns < 0 if cannot immediately accept a connection, uses
 * select to determine if can select or not.
 */
int TcpServerSocket::acceptNB(Connection& con) {
   fd_set rdfds;
   FD_ZERO(&rdfds);
   FD_SET(_serverConn->_connId, &rdfds);
   struct timeval tv;
   tv.tv_usec = 0;
   tv.tv_sec = 0;

   if (select(_serverConn->_connId + 1, &rdfds, NULL, NULL, &tv) > 0) {
      if ((con._connId = ::accept(_serverConn->_connId, (SA*) con._connAddr,
                                  &con._connAddrLen)) < 0) {
         return -ERRNO;
      }
      cpLog(LOG_DEBUG_STACK, "Connection from %s", con.getDescription().c_str());
      con.setState();

      // Set ToS and Priority
      vsetPrio(con._connId, _tos, _skb_priority, "TcpServerSocket::acceptNB");

      return con._connId;
   }
   return 0;
}


void TcpServerSocket::addToFdSet ( fd_set* set ) {
   FD_SET(getSocketFD(), set);
}

int TcpServerSocket::getMaxFD ( int prevMax) {
   if (prevMax < getSocketFD()) {
      return getSocketFD();
   }
   return prevMax;
}

bool TcpServerSocket::checkIfSet ( fd_set* set ) {
   return !!(FD_ISSET(getSocketFD(), set));
}

int TcpServerSocket::accept(Connection& con) {
   if ((con._connId = ::accept(_serverConn->_connId, (SA*) con._connAddr, &con._connAddrLen)) < 0) {
      cpLog(LOG_ERR, "Failed to accept the connection, reason:%s", VSTRERROR);
   }
   else {
      cpLog(LOG_DEBUG_STACK, "Connection from %s", con.getDescription().c_str());
      con.setState();

      // Set ToS and Priority
      vsetPrio(con._connId, _tos, _skb_priority, "TcpServerSocket::accept");
   }
   return con._connId;
}

void TcpServerSocket::close() {
#ifndef WIN32
   ::close(_serverConn->_connId);
#else
   ::closesocket(_serverConn->_connId);
#endif
    _serverConn->_connId = -1; //Ensure it is not used later.
}

#endif
