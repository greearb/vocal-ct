
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





static const char* const TcpServerSocket_cxx_Version =
    "$Id: Tcp_ServerSocket.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#ifndef __vxworks


#include "global.h"
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
#include "NetworkConfig.hxx"

#define LISTENQ   15
TcpServerSocket::TcpServerSocket(const string& _local_ip,
                                 const string& local_dev_to_bind_to,
                                 int servPort)
   throw (VNetworkException&)
{
    local_ip = _local_ip;
    listenOn(local_ip, local_dev_to_bind_to, servPort);
}

void
TcpServerSocket::listenOn(const string& local_ip, const string& local_dev_to_bind_to,
                          int servPort) throw (VNetworkException&)
{
    struct addrinfo hints, *res, *sRes;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = NetworkConfig::instance().getAddrFamily();
    hints.ai_socktype = SOCK_STREAM;
    char serv[56], err;
    sprintf(serv, "%u", servPort);
    const char* lip = NULL;
    if (local_ip.size()) {
       lip = local_ip.c_str();
    }

    if((err= getaddrinfo(lip, serv, &hints, &res)) != 0)
    {
        char buf[256];
        sprintf(buf, "getaddrinfo failed, reason:%s", gai_strerror(errno));
        cpLog(LOG_ERR, buf);
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }

    sRes = res;
    do
    {
        _serverConn._connId = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
         if (_serverConn._connId < 0)
         {
             continue;
         }
         int on = 1;
#ifndef WIN32
         if ( setsockopt ( _serverConn._connId, SOL_SOCKET, SO_REUSEADDR,
                      reinterpret_cast<sockbuf_t *>(&on), sizeof ( on )) )
         {
             // this is an error -- can't set it
             cpLog(LOG_ALERT, "setsockopt failed (REUSEADDR), reason: %s", strerror(errno));
         }
#endif


         // 16/1/04 fpi		  
         // tbr
         // todo
         // Win32 WorkAround
         // Note: I think this code is not useful,
         // binding to a specific ip binds on the device
         // that has the ip assigned

         // It's useful on Linux, but only in limited situations. --Ben
#ifdef __linux__
         if (local_dev_to_bind_to.size()) {
            // Bind to specific device.
            char dv[15 + 1];
            strncpy(dv, local_dev_to_bind_to.c_str(), 15);
            if (setsockopt(_serverConn._connId, SOL_SOCKET, SO_BINDTODEVICE,
                           dv, 15 + 1)) {
               cpLog(LOG_ERR, "ERROR:  setsockopt (BINDTODEVICE), dev: %s  error: %s\n",
                     dv, strerror(errno));
            }
         }
#endif

        if (::bind(_serverConn._connId, res->ai_addr, res->ai_addrlen) == 0)
        {

            //Success
            delete []_serverConn._connAddr;
            _serverConn._connAddr = (struct sockaddr*) new char[res->ai_addrlen];
            memcpy(_serverConn._connAddr, res->ai_addr, res->ai_addrlen);

            char tmp_addr[80];
            struct sockaddr_in* sin = (struct sockaddr_in*)(res->ai_addr);
            inet_ntop(res->ai_family, &(sin->sin_addr.s_addr), tmp_addr, 80);
            tmp_addr[79] = 0;
            curLocalIp = tmp_addr;

            cpLog(LOG_INFO, "(%s) TCP server locally bound to %s:%d",
                  (res->ai_family == PF_INET) ? "IPv4" : "IPv6",
                  curLocalIp.c_str(), servPort);
            break;
        }
#ifndef WIN32
        ::close(_serverConn._connId);
#else
        // 25/11/03 fpi
        // WorkAround Win32
        ::closesocket(_serverConn._connId);
#endif
        _serverConn._connId = -1; //Ensure it is not used later.
    } while((res = res->ai_next) != NULL);

    freeaddrinfo(sRes);

    if(res == NULL)
    {
        char buf[256];
        sprintf(buf, "Failed to initialize TCP server on ip: [%s]:%d, reason: %s",
                local_ip.c_str(), servPort, strerror(errno));
        cpLog(LOG_ALERT, "%s", buf);
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }

    if (::listen(_serverConn._connId, LISTENQ))
    {
        char buf[256];
        sprintf(buf, "listen failed, reason:%s", strerror(errno));
        cpLog(LOG_ALERT, "%s", buf);
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }
}

TcpServerSocket::TcpServerSocket(const TcpServerSocket& other)
{
    _serverConn = other._serverConn;
    _clientConn = other._clientConn;
    local_ip = other.local_ip;
}

TcpServerSocket&
TcpServerSocket::operator=(TcpServerSocket& other)
{

    if (this != &other)
    {
        local_ip = other.local_ip;
        _serverConn = other._serverConn;
        _clientConn = other._clientConn;
    }
    return *this;
}

TcpServerSocket::~TcpServerSocket()
{
    close();
}


int
TcpServerSocket::accept(Connection& con) throw (VNetworkException&)
{
    if ((con._connId = ::accept(_serverConn._connId, (SA*) con._connAddr, &con._connAddrLen)) < 0)
    {
        char buf[256];
        sprintf(buf, "Failed to accept the connection, reason:%s", strerror(errno));
        cpLog(LOG_DEBUG, buf);
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }
    cpLog(LOG_DEBUG_STACK, "Connection from %s", con.getDescription().c_str());
    con._live = true;
    con.setState();
    return con._connId;
}

void
TcpServerSocket::close()
{
    #if !defined(WIN32)
    ::close(_serverConn.getConnId());
    #else
    ::closesocket(_serverConn.getConnId());
    #endif
}

#endif
