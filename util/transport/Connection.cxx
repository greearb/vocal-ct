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

static const char* const Connection_cxx_version =
    "$Id: Connection.cxx,v 1.8 2006/02/24 22:27:52 greear Exp $";

#ifndef __vxworks

#include "global.h"
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>
#include <arpa/inet.h>

#ifndef __vxworks
#include <sys/time.h>
#else
#include "vsock.hxx"
#endif

#include "VNetworkException.hxx"
#include "cpLog.h"
#include "Connection.hxx"


bool Connection::_init = false;


Connection::Connection(bool blocking)
    : _connId(-1), _connAddrLen(0), 
      _connAddr(0), _blocking(blocking),
      closeOnDestruct(true)
{
    initialize();
}


Connection::Connection(int conId, bool blocking)
    : _connId(conId), _connAddrLen(0), 
      _connAddr(0), _blocking(blocking),
      closeOnDestruct(true)
{
    initialize();
}

// TODO:  I don't like this, should remove it.
Connection::Connection(const Connection& other, bool on_purpose) {
    _connId = other._connId;
    _connAddrLen = other._connAddrLen;
    _blocking = other._blocking;
    if (other._connAddr) {
        _connAddr = (struct sockaddr*) new char[_connAddrLen];
        memcpy((void*)_connAddr, (void*)other._connAddr, _connAddrLen);
    }
    else {
        _connAddr = NULL;
    }
    setState();
    closeOnDestruct = false;
}

Connection::~Connection() {
    if (closeOnDestruct) {
        close();
    }
    delete []_connAddr;
}


// TODO:  I don't like this, should remove it.
Connection& Connection::operator=(const Connection& other) {
    if (this != &other) {
	_connId = other._connId;
	_connAddrLen = other._connAddrLen;
	_blocking = other._blocking;
        if (other._connAddr) {
            delete []_connAddr;
            _connAddr = (struct sockaddr*) new char[_connAddrLen];
	    memcpy((void*)_connAddr, (void*)other._connAddr, _connAddrLen);
        }
        else {
            _connAddr = NULL;
        }
	setState();
    }
    return *this;
}


void
Connection::initialize() {
    if (!_init) {

#if !defined(__vxworks) && !defined(WIN32)
        // setup SIGPIPE handler
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
            cpLog(LOG_ALERT,
                  "Warning: Can't ignore SIGPIPE, broken pipes will exit program");
#endif
        _init = true;
    }
    _connAddr = (struct sockaddr*) new char[sizeof(struct sockaddr)];
    _connAddrLen = sizeof(struct sockaddr);
    memset(_connAddr, 0, _connAddrLen);
}

// Call readNB first to fill buffer.
int Connection::getLine(char* rbuf, int maxlen) {
    int i;
    int mx = min(maxlen, rcvBuf.getCurLen());
    for (i = 0; i<mx; i++) {
        unsigned char c = rcvBuf.charAt(i);
        if (c == '\n') {
            rcvBuf.dropFromTail(i);
            rbuf[i] = 0;
            return i;
        }
        else {
            rbuf[i] = (char)(c);
        }   
    }

    // No newline found, but maybe our buffer is full anyway?

    if (i == maxlen) {
        rcvBuf.dropFromTail(i);
        rbuf[i] = 0;
        return i;
    }

    // Buffer is not full, and we have no newline, so return -1 indicating
    // no line was found.
    rbuf[0] = 0;
    return -1;
}


int Connection::read() {
    return iread();
}

int Connection::write() {
    return iwrite();
}

int Connection::iclose() {
    assert(!shouldCloseOnDestruct());
#ifndef WIN32
    return ::close(_connId);
#else
    return closesocket(_connId);
#endif
}


ssize_t Connection::iread() {
    return rcvBuf.read(_connId, 65536, NULL);
}


ssize_t Connection::iwrite() {
    return outBuf.write(_connId);
}

#ifdef _WIN32
#define snprintf _snprintf
#endif

string
Connection::getDescription() const
{
    string retStr;
    char hName[256];
    char portBuf[256];
    char descBuf[256];
    SA* sa = (SA*) _connAddr;
    switch (sa->sa_family)
    {
        case AF_INET:
        {
            struct sockaddr_in* sin = (struct sockaddr_in*) sa;
            if(inet_ntop(AF_INET, &sin->sin_addr, hName, 256) == 0)
            {
                cpLog(LOG_ERR, "inet_ntop failed");
            };
            if(ntohs(sin->sin_port) != 0)
            {
               sprintf(portBuf, "%d", ntohs(sin->sin_port));
            }
            snprintf(descBuf, 256, "%s:%s", hName, portBuf);
            retStr = descBuf;
        }
        break;
        case AF_INET6:
        {
            struct sockaddr_in6* sin = (struct sockaddr_in6*)(sa);
            if(inet_ntop(AF_INET6, &sin->sin6_addr, hName, 256) == 0)
            {
                cpLog(LOG_ERR, "inet_ntop failed");
            };
            if(ntohs(sin->sin6_port) != 0)
            {
               sprintf(portBuf, "%d", ntohs(sin->sin6_port));
            }
            snprintf(descBuf, 256, "%s:%s", hName, portBuf);
            retStr = descBuf;
        }
        break;
        default:
            retStr = "Unknown";
        break;
    }
    return retStr;
}

string Connection::getPeerIp() const {
    string retStr;
    char hName[256];
    SA* sa = (SA*) _connAddr;
    switch (sa->sa_family)
    {
        case AF_INET:
        {
            struct sockaddr_in* sin = (struct sockaddr_in*) sa;
            if(inet_ntop(AF_INET, &sin->sin_addr, hName, 256) == 0)
            {
                cpLog(LOG_ERR, "inet_ntop failed");
            }
            else
            {
                retStr = hName;
            }
        }
        break;
        case AF_INET6:
        {
            struct sockaddr_in6* sin = (struct sockaddr_in6*)(sa);
            if(inet_ntop(AF_INET6, &sin->sin6_addr, hName, 256) == 0)
            {
                cpLog(LOG_ERR, "inet_ntop failed");
            }
            else
            {
                retStr = hName;
            }
        }
        break;
        default:
        retStr = "Unknown";
        break;
    }
    return retStr;
}

int Connection::getPeerPort() const {
    int retPort = -1;
    SA* sa = (SA*)  _connAddr;
    switch (sa->sa_family) {
        case AF_INET: {
            struct sockaddr_in* sin = (struct sockaddr_in*) sa;
            retPort = (ntohs(sin->sin_port));
        }
        break;
        case AF_INET6: {
            struct sockaddr_in6* sin = (struct sockaddr_in6*) sa;
            retPort = (ntohs(sin->sin6_port));
        } 
        break;
    }
    return retPort;
}

void Connection::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      uint64 now) {

   if (!isLive()) {
      return;
   }

   if (checkIfSet(input_fds)) {
      if (_inProgress) {
         // Connect in progress, and looks like it completed, check success.
         int rv;
         int so_err = 0;
         socklen_t slen = sizeof(int);
         rv = getsockopt(getSocketFD(), SOL_SOCKET, SO_ERROR, &so_err, &slen);
         if (rv < 0) {
            cpLog(LOG_ERR, "ERROR: getsockopt failed w/val: %d: %s\n",
                  rv, strerror(errno));
            close();
            return;
         }
         else {
            if (so_err != 0) {
               cpLog(LOG_ERR, "WARNING: Could not establish connection: %d: %s\n",
                     so_err, strerror(so_err));
               close();
               return;
            }
            else {
               // Success, set TOS here if we ever want to do that sort of thing
               _inProgress = false;
            }
         }
      }
      else {
         read();
      }
   }
   if (!_inProgress) {
      write();
   }
   if (checkIfSet(exc_fds)) {
      cpLog(LOG_ERR, "Closing connection: %s due to fdset exception.\n",
            toString().c_str());
      close();
   }
}


// Clear out our buffers, does not attempt to flush.
void Connection::clear() {
   outBuf.clear();
   rcvBuf.clear();
}


int Connection::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                       int& maxdesc, uint64& timeout, uint64 now) {

   if (!isLive()) {
      return 0;
   }

   addToFdSet(input_fds);
   if (needsToWrite()) {
      addToFdSet(output_fds);
   }
   addToFdSet(exc_fds);

   if (getSocketFD() > maxdesc) {
      maxdesc = getSocketFD();
   }
   return 0;
}

// Queue it for send.  It will be written as soon as possible.
int Connection::queueSendData(const char* data, int len) {
    //TODO:  Consider putting a max limit on this (4-8MB?) so that
    //    we are sure not to blow memory on a backed up TCP connection?
    outBuf.append((const unsigned char*)(data), len);
    write(); // Flush if we can
    return len;
}


// Returns the number of bytes actually peeked, and
// buf will be null-terminated.
int Connection::peekRcvdBytes(unsigned char* buf, int mx_buf_len) {
    int mx = min(rcvBuf.getCurLen(), mx_buf_len - 1);
    rcvBuf.peekBytes(buf, mx);
    buf[mx] = 0;
    return mx;
}

int Connection::close() {
    cpLog(LOG_DEBUG_STACK, "Closing connection %d", _connId);
    int err = 0;
    if (_connId > 2) {
        err = iclose();
        if (err) {
            cpLog(LOG_ERR, "Error closing connection %d", _connId);
        }
    }
    _connId = -1;
    return err;
}

void Connection::setState() {
    if (_connId) {
#ifndef WIN32
        if (!_blocking) {
            fcntl(_connId, F_SETFL, O_NONBLOCK);
        }
#else
        unsigned long non_blocking = _blocking ? 0 : 1;
        if (ioctlsocket(_connId, FIONBIO, &non_blocking) == SOCKET_ERROR) {
            cpLog(LOG_ERR, "Error setting Connection FIONBIO: %d", WSAGetLastError());
        }
#endif
    }
}

void Connection::addToFdSet ( fd_set* set ) {
    FD_SET(getSocketFD(), set);
}

int Connection::getMaxFD ( int prevMax) {
    if (getSocketFD() > prevMax) {
        return getSocketFD();
    }
    return prevMax;
}

bool Connection::checkIfSet ( fd_set* set ) {
    return FD_ISSET(getSocketFD(), set);
}


#endif
