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

static const char* const TcpClientSocket_cxx_Version =
    "$Id: Tcp_ClientSocket.cxx,v 1.3 2004/05/07 17:30:46 greear Exp $";

#ifndef __vxworks

#include "global.h"
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#ifndef WIN32
#include <strings.h>
#else
#include <string.h>
#endif

#include "VNetworkException.hxx"
#include "Tcp_ClientSocket.hxx"
#include "VEnvVar.hxx"
#include "NetworkAddress.h"
#include "NetworkConfig.hxx"
#include "cpLog.h"

TcpClientSocket::TcpClientSocket(const string& hostName,
                                 const string& _local_dev_to_bind_to,
                                 const string& _local_ip_to_bind_to,
                                 bool closeCon, bool blocking)
    :
    local_dev_to_bind_to(_local_dev_to_bind_to),
    local_ip_to_bind_to(_local_ip_to_bind_to),
    _hostName(hostName),
    _serverPort( -1),
    _closeCon(closeCon),
    _blocking(blocking)
{
    initalize();
}

TcpClientSocket::TcpClientSocket(const string& hostName, int servPort,
                                 const string& _local_dev_to_bind_to,
                                 const string& _local_ip_to_bind_to,
                                 bool closeCon, bool blocking)
    :
    local_dev_to_bind_to(_local_dev_to_bind_to),
    local_ip_to_bind_to(_local_ip_to_bind_to),
    _hostName(hostName),
    _serverPort(servPort),
    _closeCon(closeCon),
    _blocking(blocking)
{
    initalize();
}

TcpClientSocket::TcpClientSocket(const NetworkAddress& server,
                                 const string& _local_dev_to_bind_to,
                                 const string& _local_ip_to_bind_to,
                                 bool closeCon, bool blocking)
    :
    local_dev_to_bind_to(_local_dev_to_bind_to),
    local_ip_to_bind_to(_local_ip_to_bind_to),
    _closeCon(closeCon),
    _blocking(blocking)
{
    _hostName = server.getHostName();
    _serverPort = server.getPort();
    cpLog(LOG_DEBUG_STACK, "%s %d", _hostName.c_str(), _serverPort);
    if (_serverPort == -1 )
    {
        _serverPort = VEnvVar::VPS_PORT;
    }
    initalize();
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


void
TcpClientSocket::initalize()
{
    _conn = new Connection(_blocking);
}

TcpClientSocket::~TcpClientSocket()
{
    close();
}


bool TcpClientSocket::isConnected() const {
    if (_conn->isLive()) {
        if (!_conn->isConnectInProgress()) {
            return true;
        }
    }
    return false;
}


void
TcpClientSocket::connect() throw (VNetworkException&)
{
    struct addrinfo hints, *res, *tSave;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = NetworkConfig::instance().getAddrFamily();
    hints.ai_socktype = SOCK_STREAM;
    int err=0;
    char pBuf[56];
    int myerrno = 0;

    sprintf(pBuf, "%d", _serverPort);
    if((err = getaddrinfo(_hostName.c_str(), pBuf, &hints, &res)) != 0) {
        char buf[256];
        sprintf(buf, "Failed to getaddrinfo for server %s:%d, reason %s",
            _hostName.c_str(), 
            _serverPort,
            gai_strerror(errno));
        cpLog(LOG_ERR, buf);
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }
    tSave = res;
    do {
        assert(_conn->_connId < 0); // Make sure we don't leak sockets

        _conn->_connId = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (_conn->_connId < 0) {
            char buf[256];
            sprintf(buf, "Failed to create socket: reason %s",
                strerror(errno));
            cpLog(LOG_DEBUG, buf);
            continue;
        }

        // 16/1/04 fpi		  
        // tbr
        // todo
        // Win32 WorkAround
        // Note: I think this code is not useful,
        // binding to a specific ip binds on the device
        // that has the ip assigned

        // It is useful in some cases on Linux, at least. --Ben
#ifdef __linux__
        // Optionally, bind this to the local interface.
        if (local_dev_to_bind_to.size()) {
            // Bind to specific device.
            char dv[15 + 1];
            strncpy(dv, local_dev_to_bind_to.c_str(), 15);
            if (setsockopt(_conn->_connId, SOL_SOCKET, SO_BINDTODEVICE,
                           dv, 15 + 1)) {
                cpLog(LOG_ERR, "ERROR:  setsockopt (BINDTODEVICE), dev: %s  error: %s\n",
                      dv, strerror(errno));
            }
        }
#endif

        // Bind to the local IP
        if (local_ip_to_bind_to.size()) {
            struct addrinfo hints2, *res2;
            memset(&hints2, 0, sizeof(hints2));
            hints2.ai_flags = AI_PASSIVE;
            hints2.ai_family = NetworkConfig::instance().getAddrFamily();
            hints2.ai_socktype = SOCK_STREAM;
            const char* lip = local_ip_to_bind_to.c_str();

            if ((err = getaddrinfo(lip, NULL, &hints2, &res2)) != 0) {
                char buf[256];
                sprintf(buf, "getaddrinfo failed, reason:%s", gai_strerror(errno));
                cpLog(LOG_ERR, buf);
                throw VNetworkException(buf, __FILE__, __LINE__, errno);
            }
            else {
                if (::bind(_conn->_connId, res2->ai_addr, res2->ai_addrlen) != 0) {
                    char buf[256];
                    sprintf(buf, "bind failed, reason:%s", gai_strerror(errno));
                    cpLog(LOG_ERR, buf);
                    throw VNetworkException(buf, __FILE__, __LINE__, errno);
                }
            }
        }

        int rv = ::connect(_conn->_connId, res->ai_addr, res->ai_addrlen);
        if (rv >= 0) {
            ///Success
            char descBuf[256];
            cpLog(LOG_DEBUG, "Connected to %s", connectionDesc(res, descBuf, 256));
            delete []_conn->_connAddr;
            _conn->_connAddr = (struct sockaddr*) new char[res->ai_addrlen];
            memcpy((_conn->_connAddr), (res->ai_addr), res->ai_addrlen);
            cpLog(LOG_DEBUG, "SIze:%d, size2:%d",
                  sizeof(*_conn->_connAddr), res->ai_addrlen);
            _conn->_connAddrLen = res->ai_addrlen;
            break;
        }
        else {
            myerrno = errno;
            // If we are non-blocking, then EINPROGRESS is OK
            if (myerrno == EINPROGRESS) {
                _conn->setConnectInProgress(true);
                break;
            }
        }

        _conn->close();
    } while ((res = res->ai_next) != 0);

    if (res == 0) {
        char buf[256];
        sprintf(buf, "Failed to connect to server %s:%d, reason %s (%d)",
            _hostName.c_str(), 
            _serverPort,
            gai_strerror(myerrno), myerrno);
        cpLog(LOG_ERR, buf);
        _conn->close();
        throw VNetworkException(buf, __FILE__, __LINE__, errno);
    }
    freeaddrinfo(tSave);
    _conn->setState();
}

void
TcpClientSocket::close()
{
    if (_closeCon && _conn->getConnId() > 2)
        _conn->close();
}

const char*
TcpClientSocket::connectionDesc(struct addrinfo* laddr, char* descBuf, int bufLen) const
{
    assert(laddr);
    char hName[256];
    char portBuf[56];
    hName[0] = '\0';
    portBuf[0] = '\0';
    switch(laddr->ai_family)
    {
        case AF_INET:
        {
            cpLog(LOG_DEBUG, "IPv4");
            struct sockaddr_in* sin = (struct sockaddr_in*)(laddr->ai_addr);
            if(inet_ntop(laddr->ai_family, &sin->sin_addr, hName, 256) == 0)
            {
                return 0;
            };
            if(ntohs(sin->sin_port) != 0)
            {
               sprintf(portBuf, "%d", ntohs(sin->sin_port)); 
            }
            snprintf(descBuf, bufLen, "%s:%s", hName, portBuf);
        }
        break;
        case AF_INET6:
        {
            cpLog(LOG_DEBUG, "IPv6");
            struct sockaddr_in6* sin = (struct sockaddr_in6*)(laddr->ai_addr);
            if(inet_ntop(laddr->ai_family, &sin->sin6_addr, hName, 256) == 0)
            {
                return 0;
            };
            if(ntohs(sin->sin6_port) != 0)
            {
               sprintf(portBuf, "%d", ntohs(sin->sin6_port)); 
            }
            snprintf(descBuf, bufLen, "%s:%s", hName, portBuf);
        }
        break;
        default:
            snprintf(descBuf, bufLen, "Unknown");
        break;
    }
    return descBuf;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
