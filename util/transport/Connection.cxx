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
    "$Id: Connection.cxx,v 1.4 2004/05/07 17:30:46 greear Exp $";

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
    : _connId(-1), _live(false), _connAddrLen(0), 
      _connAddr(0), _blocking(blocking),
      closeOnDestruct(true)
{
    initialize();
}


Connection::Connection(int conId, bool blocking)
    : _connId(conId), _live(true), _connAddrLen(0), 
      _connAddr(0), _blocking(blocking),
      closeOnDestruct(true)
{
    initialize();
}

Connection::Connection(const Connection& other, bool on_purpose) {
    _connId = other._connId;
    _live = other._live;
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


Connection& Connection::operator=(const Connection& other) {
    if (this != &other) {
	_connId = other._connId;
	_live = other._live;
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


int Connection::getLine(char* rbuf, int maxlen) {
    assert(maxlen > MAXLINE); // Force calling code to give us an appropriately large buffer.
    int i;
    for (i = 0; i<sofar; i++) {
        if (buf[i] == '\n') {
            memcpy(rbuf, buf, i);
            buf[i] = 0; // Terminate string, overwriting the newline
            memmove(buf, buf + i + 1, MAXLINE - (i + 1));
            sofar -= (i + 1);
            return i;
        }
    }

    // No newline found, but maybe our buffer is full anyway?
    assert(sofar <= MAXLINE);

    if (sofar == MAXLINE) {
        memcpy(rbuf, buf, sofar);
        rbuf[sofar] = 0;
        sofar = 0; // Clear entire buffer
        return sofar;
    }

    // Buffer is not full, and we have no newline, so return -1 indicating
    // no line was found.
    rbuf[0] = 0;
    return -1;
}

int Connection::readNB() throw (VNetworkException&) {
    int rc;

    rc = readn(buf + sofar, MAXLINE - sofar);
    if (rc < 0) {
        if ((errno == EINTR) || (errno == EAGAIN)) {
            // Not a problem.
        }
        else {
            return -errno; // Read failure
        }
    }
    else if (rc == 0) {
        // EOF reached, return what we have.
        buf[sofar] = 0; //Terminate string
        return sofar;
    }
    else {
        sofar += rc;
    }

    // If here, we MAY have a complete line (or even multiple lines)
    return sofar;
}


int
Connection::readLine(void* dataRead, size_t maxlen, int& bytesRead) throw (VNetworkException&)
{
    char c;
    int rc, i;
    char* ptr = (char*)dataRead;

    bytesRead = 0;
    for (i = 1; i <= (int)maxlen; i++)
    {
        if ((rc = effRead(&c)) == 1)
        {
            *ptr++ = c;
            bytesRead++;
            if (c == '\n') break;
        }
        else if (rc == 0)
        {
            if ( i == 1) return 0;
            else break;
        }
        else
        {
            return -1;
        }
    }
    *ptr = 0;
    dataRead = ptr;
    return ((i == 1) ? 0 : i);
}

int
Connection::readn(void *dataRead, size_t nchar, int & bRead) throw (VNetworkException&)
{
    size_t nleft;
    ssize_t nread;
    char *ptr = (char*)dataRead;

    nleft = nchar;

    while (nleft > 0)
    {
        if ((nread = iread(ptr, nleft)) < 0)
        {
            if ((errno == EINTR) || (errno == EAGAIN))
            {
                //cpLog(LOG_DEBUG, "Received EINTR or EAGAIN, keep trying");
                nread = 0;
            }
            else
            {
                return -1;
            }
        }
        else if (nread == 0)
        {
            //NO more data
            return 0;
        }
        nleft -= nread;
        ptr += nread;
        bRead += nread;
    }
    return (1);
}

int
Connection::readn(void *dataRead, size_t nchar) throw (VNetworkException&)
{
    if (dataRead == 0)
        return -1;
    return (iread(static_cast < char* > (dataRead), nchar));
}

void
Connection::writeData(string& data) throw (VNetworkException&)
{
    char *ptr = (char*)data.c_str();
    size_t nleft = data.size();
    writeData(ptr, nleft);
}

void
Connection::writeData(void* data, size_t n) throw (VNetworkException&)
{
    size_t nleft;
    ssize_t nwritten;
    char *ptr = (char*)data;
    nleft = n;

    while (nleft > 0)
    {
        if ((nwritten = iwrite(ptr, nleft)) <= 0)
        {
            if ((errno == EINTR) || (errno == EAGAIN))
            {
                //cpLog(LOG_DEBUG, "Received EINTR or EAGAIN, keep trying");
                nwritten = 0;   //Write again
            }

            else
            {
                char buf[256];
                sprintf(buf, "Failed to write data,reason:%s", strerror(errno));
                cpLog(LOG_ALERT, buf);
                throw VNetworkException(buf, __FILE__, __LINE__, errno);
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    //cpLog(LOG_DEBUG, "Bytes written:%d" , nwritten);
}


// Inefficient, reads one char at a time!!
ssize_t
Connection::effRead(char* ptr)
{
    int readCnt = 0;
    char* readPtr;
    char readBuf[1];
    if (readCnt <= 0)
    {
        while (1)
        {
            if ((readCnt = iread(readBuf, sizeof(readBuf))) < 0)
            {
                if ((errno == EINTR) || (errno == EAGAIN))
                {
                    //cpLog(LOG_DEBUG, "Received EINTR or EAGAIN, keep trying");
                    usleep(20);
                    continue;
                }
                char buf[256];
                sprintf(buf, "Failed to read data, reason:%s", strerror(errno));
                cpLog(LOG_ERR, buf);
                return -1 ;
            }
            else if (readCnt == 0)
            {
                return 0;
            }
            readPtr = readBuf;
            break;
        }
    }
    readCnt--;
    *ptr = *readPtr++;
    return 1;
}


int
Connection::iclose()
{
    assert(!shouldCloseOnDestruct());
#ifndef WIN32
    return ::close(_connId);
#else
    return closesocket(_connId);
#endif
}


ssize_t
Connection::iread(char* buf, size_t count)
{
#ifndef WIN32
    return ::read(_connId, buf, count);
#else
    return ::recv(_connId, buf, count, 0);
#endif
}


ssize_t
Connection::iwrite(char* buf, size_t count)
{
#ifndef WIN32
    return ::write(_connId, buf, count);
#else
    return ::send(_connId, buf, count, 0);
#endif
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

string
Connection::getIp() const
{
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

int
Connection::getPort() const
{
    int retPort = -1;
    SA* sa = (SA*)  _connAddr;
    switch (sa->sa_family)
    {
        case AF_INET:
        {
            struct sockaddr_in* sin = (struct sockaddr_in*) sa;
            retPort = (ntohs(sin->sin_port));
        }
        break;
        case AF_INET6:
        {
            struct sockaddr_in6* sin = (struct sockaddr_in6*) sa;
            retPort = (ntohs(sin->sin6_port));
        } 
        break;
    }
    return retPort;
}

int
Connection::close()
{
    cpLog(LOG_DEBUG_STACK, "Closing connection %d", _connId);
    int err = 0;
    if(_connId > 2)
    {
        err = iclose();
        if (err)
        {
            cpLog(LOG_ERR, "Error closing connection %d", _connId);
        }
    }
    _live = false;
    _connId = -1;
    return err;
}

void
Connection::setState()
{
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
        _live = true;
    }
    else {
        _live = false;
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


bool
Connection::isReadReady(int seconds, int mSecconds) const
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    //cpLog(LOG_DEBUG, "Checking connection %s", getDescription().c_str());
    FD_ZERO(&rfds);
    FD_SET(getConnId(), &rfds);
    tv.tv_sec = seconds;
    tv.tv_usec = mSecconds;
    int connId = getConnId();
    int maxFd = connId;
    retval = select(maxFd + 1, &rfds, NULL, NULL, &tv);
    if (retval > 0 && FD_ISSET(connId, &rfds))
    {
        cpLog(LOG_DEBUG_STACK, "Data is ready on fd %d : %s", connId, getDescription().c_str());
        return true;
    }
    return false;
}

void
Connection::deepCopy(const Connection& src, char** bufPtr, int* bufLenPtr)
{
    _connId = src._connId;
    _live = src._live;
    _connAddrLen = src._connAddrLen;
    _connAddr = src._connAddr;
    _blocking = src._blocking;
    _init = src._init;
    _connAddr = (struct sockaddr*) new char[_connAddrLen];
    memcpy((void*)_connAddr, (void*)src._connAddr, _connAddrLen);

}

#endif
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
