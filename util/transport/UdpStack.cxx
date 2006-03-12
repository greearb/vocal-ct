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


/* TODO List
 * - add sendTo function to allow you to specifiy different destinations
 * - add recvFrom function that tell you who the packet came from 
 * - add non blockinge version fo send and receive
 * - derive "ReliableUDP" stack that takes care of retransmissions
 * - look into using MSG_ERRQUEUE to check for ICMP errors
 * - check portability 
 * - support IP6 
 * - stress test 
 */

#include "global.h"

// 25/11/03 fpi
// WorkAround Win32
#ifdef WIN32
#include "tpipv6.h"
#endif

#include <fstream>
#include <assert.h>
#include <string.h> // for memset
#include <errno.h>
#include <iostream>
#include <stdio.h>

#ifndef __MINGW32__
#include <netdb.h>
#include <netinet/in.h> // for struct sockaddr_in
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/uio.h>
#endif

#include <sys/types.h>
#include <stdlib.h>
#include <strstream>
#include "VTime.hxx"
#include <unistd.h>

#ifndef __vxworks
#include <fcntl.h>
#endif
#ifdef __APPLE__
#include "TransportCommon.hxx"
#endif
#include "vovida-endian.h"

#if defined(__svr4__)
#ifndef sparc
#include <xil/xil.h>
#endif

#if (XIL_API_MINOR_VERSION - 0 == 3)
typedef int socklen_t;
#endif
#endif

#if defined __FreeBSD__
#include <isc/eventlib.h>
#endif

#ifdef __APPLE__
typedef int socklen_t;
#endif

#include "UdpStack.hxx"
#include "cpLog.h"
#include "vsock.hxx"
#include "NetworkConfig.hxx"

// 16/1/04 fpi
// WorkAround Win32
#ifdef WIN32

struct iovec {
	void *iov_base;
	size_t iov_len;
};

struct msghdr
{
      void      *msg_name;        /* ptr to socket address structure */
      socklen_t  msg_namelen;     /* size of socket address structure */
      struct iovec  *msg_iov;     /* scatter/gather array */
      size_t     msg_iovlen;      /* # elements in msg_iov */
      void      *msg_control;     /* ancillary data */
      socklen_t  msg_controllen;  /* ancillary data buffer length */
      int        msg_flags;       /* flags on received message */
};

struct cmsghdr
{
      socklen_t  cmsg_len;   /* #bytes, including this header */
      int        cmsg_level; /* originating protocol */
      int        cmsg_type;  /* protocol-specific type */
                 /* followed by unsigned char cmsg_data[]; */
};

#define MSG_CTRUNC 0x20
typedef unsigned int  uintptr_t;

#endif

// This is a file that contains multicast definitions for window support
// contact nismail@cisco.com
#ifdef WIN32
#include "W32McastCfg.hxx"
#endif


static const char separator[7] = "\n****\n";

UdpStack::UdpStack ( uint16 tos, uint32 priority,
                     bool isBlocking, /* Are we a blocking or non-blocking socket? */
                     const string& local_ip,
                     const string& device_to_bind_to,
                     const NetworkAddress* desHost
                     /* null if this is the server */ ,
                     int minPort,
                     int maxPort,
                     UdpMode udpMode,
                     bool log_flag,
                     bool isMulticast)
        :
        packetLossProbability( float(0.0) ),
        numBytesReceived (0),
        numPacketsReceived (0),
        numBytesTransmitted (0),
        numPacketsTransmitted (0),
        mode (sendrecv),
        socketFd(-1),
        localAddr(local_ip),
        remoteAddr(""),
        desiredLocalIp(local_ip),
        boundLocal(false),
        logFlag (log_flag),
        in_log(NULL),
        out_log(NULL),
        rcvCount(0),
        sndCount(0),
	blockingFlg(true),
        localDev(device_to_bind_to)
{
   _tos = tos;
   _skb_priority = priority;

   mode = udpMode;

   socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   cpLog (LOG_DEBUG_STACK, "UdpStack socketFd = %d", socketFd);
   if (socketFd < 0 ) {
      // Clean up memory before we throw the exception.
      
      strstream errMsg;
      errMsg << "UdpStack::::UdpStack error during socket creation:";
      errMsg << "Reason " << VSTRERROR << ends;
      
      cpLog(LOG_ERR, errMsg.str());
   }
   
   int buf1 = 1;   
   int rcvbuf = 0;
   int rcvbufnew = 240 * 1024;
   int sndbuf = 0;
   int len;

   char dbg[128];
   snprintf(dbg, 128, "UdpStack: %s:%i-%i",
            local_ip.c_str(), minPort, maxPort);

   // Set ToS and Priority
   vsetPrio(socketFd, _tos, _skb_priority, dbg);
   
   if (setsockopt(socketFd, SOL_SOCKET, SO_RCVBUF,
                  (char *)(&rcvbufnew), sizeof(rcvbufnew)) < 0) {
      fprintf(stderr, "setsockopt error SO_RCVBUF :%s\n",
              VSTRERROR);
   }
      
   len = sizeof(rcvbuf);
   if (getsockopt(socketFd, SOL_SOCKET, SO_RCVBUF, 
                  (char*)&rcvbuf, &len) < 0) {
      fprintf(stderr, "getsockopt error SO_RCVBUF :%s\n",
              VSTRERROR);
   }
   else {
      cpLog(LOG_DEBUG, "SO_RCVBUF = %d, rcvbuflen  =%d" , rcvbuf, len);
   }
   
   len = sizeof(sndbuf);
   if (getsockopt(socketFd, SOL_SOCKET, SO_SNDBUF, 
                  (char*)&sndbuf, &len) < 0) {
      fprintf(stderr, "getsockopt error SO_SNDBUF :%s\n",
              VSTRERROR);
   }
   else {
      cpLog(LOG_DEBUG, "SO_SNDBUF = %d, sndbuflen = %d" , sndbuf, &len);
   }
   
   
   if (isMulticast) {
      // set option to get rid of the "Address already in use" error
      if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR,
                     (char*)&buf1, sizeof(buf1)) < 0) {
         fprintf(stderr, "setsockopt error SO_REUSEADDR :%s",
                 VSTRERROR);
      }
      
#if defined(__FreeBSD__) || defined(__APPLE__) || defined(__FreeBSD__)
      
      if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEPORT,
                     (char*)&buf1, sizeof(buf1)) < 0) {
         fprintf(stderr, "setsockopt error SO_REUSEPORT :%s",
                 VSTRERROR);
      }
#endif
   }
   
   setModeBlocking(isBlocking);
   
   switch (mode) {
    case inactive : {
       cpLog(LOG_INFO, "Udp stack is inactive");
       cpLog(LOG_ERR, "desHost is saved for future use.");
       doClient(*desHost);
       break;
    }
    case sendonly : {
       if ( desHost ) {
          // set the remote address
          doClient(*desHost);
       }
       else {
          cpLog(LOG_DEBUG_STACK, "sendonly Udp stack, desHost is needed by using setDestination()");
       }
       break;
    }
    case recvonly : {
       if ( desHost ) {
          cpLog(LOG_ERR,
                "recvonly Udp stack, desHost is saved for future use.");
          doClient(*desHost);
       }
       else {
          // only receive, do bind socket to local port
          doServer(minPort, maxPort);
       }
       break;
    }
    case sendrecv : {
       if ( desHost ) {
          // receive and send,
          // bind the socket to local port and set the remote address
          doServer(minPort, maxPort);
          doClient(*desHost);
       }
       else {
          // only receive, do bind socket to local port
          doServer(minPort, maxPort);
          cpLog(LOG_DEBUG_STACK, "sendrecv Udp stack, desHost is needed by using setDestination()");
       }
       break;
    }
    default :
       cpLog(LOG_ERR, "undefined mode for udp stack");
       break;
   }//switch


// This is totally busted, need to set the file name somewhere!!
#if 0
   if (logFlag) {
      strstream logFileNameRcv;
      strstream logFileNameSnd;
      
      in_log = new ofstream(logFileNameRcv.str(), ios::app);
      if (!in_log) {
         cerr << "Cannot open file "
              << logFileNameRcv.str() << endl;
         logFileNameRcv.freeze(false);
         logFlag = false;
      }
      else {
         in_log->write ("UdpRcv\n", 7);
         strstream lcPort;
         lcPort << "localPort: " << getTxPort() << "\n" << char(0);
         in_log->write(lcPort.str(), strlen(lcPort.str()));
         lcPort.freeze(false);
         logFileNameRcv.freeze(false);
         rcvCount = 0;
      }
      
      out_log = new ofstream(logFileNameSnd.str(), ios::app);
      if (!out_log) {
         cerr << "Cannot open file "
              << logFileNameSnd.str() << endl;
         logFileNameSnd.freeze(false);
         logFlag = false;
      }
      else {
         if (! logFlag)
            logFlag = true;
         out_log->write ("UdpSnd\n", 7);
         logFileNameSnd.freeze(false);
         sndCount = 0;
      }
   }
#endif
}

int UdpStack::doServer ( int minPort, int maxPort) {
    /*
        cpLog (LOG_DEBUG_STACK, "UdpStack::doServer");
        cpLog (LOG_DEBUG_STACK, "minPort = %d, maxPort = %d", minPort, maxPort);
    */

    // this is a server
    if ( (minPort == -1) && (maxPort == -1) ) {
        minPort = 1024;
        maxPort = 65534;
    }
    if ( maxPort == -1 ) {
        maxPort = minPort;
    }

    // reset name now that the port range is defined
    strstream aName;
    aName << "-receiver-" << ":" << "[" << minPort
          << "-" << maxPort << "]" << char(0);
    setLclName( aName.str() );
    aName.freeze(false);

    // find a port to use
    int err = 0;
    int portOk = false;

    // struct addrinfo is defined in lwres/netdb.h
    // sockaddr is defined in bits/socket.h
    uint32 lip;
    if (vtoIpString(desiredLocalIp.c_str(), lip) < 0) {
       cpLog(LOG_ERR, "ERROR:  Failed to resolve local IP: %s  error: %s\n",
             desiredLocalIp.c_str(), VSTRERROR);
       return -1;
    }

    // here it assigns the port, the local port
    // & bind the addr(INADDR_ANY|lip + port) to the socket
    for (int localPort = minPort; localPort <= maxPort; localPort++ ) {
       
       struct sockaddr_in my_ip_addr;
       memset(&my_ip_addr, '\0', sizeof(my_ip_addr));
       
       my_ip_addr.sin_family = AF_INET;
       my_ip_addr.sin_addr.s_addr = htonl(lip);
       my_ip_addr.sin_port = htons(localPort);
       
       cpLog(LOG_DEBUG, "Udp bind() fd =%d, port=%s desiredLocalIp: %s",
             socketFd, localPort, desiredLocalIp.c_str());
       
       
       if (bind(socketFd, (struct sockaddr*)(&my_ip_addr), sizeof(my_ip_addr)) != 0) {
          // failed, so keep trying
          cpLog(LOG_ERR, "WARNING:  failed to bind to ip: %s(0x%x)  port: %d, error: %s\n",
                desiredLocalIp.c_str(), lip, localPort, VSTRERROR);
       }
       else {
          // successful binding occured
          cpLog(LOG_ERR, "NOTE:  bound to ip: %s(0x%x)  port: %d\n",
                desiredLocalIp.c_str(), lip, localPort);

#ifdef __linux__
          if (localDev.size()) {
             // Bind to specific device.
             char dv[15 + 1];
             strncpy(dv, localDev.c_str(), 15);
             if (setsockopt(socketFd, SOL_SOCKET, SO_BINDTODEVICE,
                            dv, 15 + 1)) {
                cpLog(LOG_ERR, "ERROR:  setsockopt (BINDTODEVICE), dev: %s  error: %s\n",
                      dv, strerror(errno));
             }
          }
#endif

          // TODO:  Should probe this, but as long as we always specify the local IP,
          //   then no big deal.
          localAddr.setHostName(vtoStringIp(lip));
          localAddr.setPort(localPort);

          boundLocal = true;
          portOk = true;

          cpLog(LOG_DEBUG, "(IPv4) Udp bound to fd = %d, addr: %s",
                socketFd, localAddr.toString().c_str());
          break;
       }
    }//for all ports

    // deal with errors
    if (!portOk) {
       // all ports are in use
       localAddr.setHostName("0.0.0.0");
       localAddr.setPort(0);
       strstream errMsg;
       errMsg << "UdpStack<" << getLclName()
              << ">::UdpStack all ports in range "
              << minPort << " to " << maxPort
              << " are in use.";
       errMsg << char(0);
       cpLog(LOG_ERR, errMsg.str());
       return -1;
    }

    // reset name now that the port is defined
    strstream aName2;
    aName2 << "-receiver-" << ":" << minPort << char(0);
    setLclName( aName2.str() );
    aName2.freeze(false);
    return 0;
}

void UdpStack::doClient ( const NetworkAddress& desHost) {
    // this is a client
    remoteAddr = desHost;
}

void
UdpStack::connectPorts() {
   if ((mode == recvonly) || (mode == inactive)) {
      cpLog(LOG_ERR, "The UdpStack is recvonly or inactive.");
      return ;
   }

   int result;

   
   struct sockaddr_in sa;
   memset(&sa, 0, sizeof(sa));
   sa.sin_family = AF_INET;
   sa.sin_port = htons(remoteAddr.getPort());
   sa.sin_addr.s_addr = htonl(remoteAddr.getIp4Address());

   // connect to server
   if ((result = connect(socketFd,
                         (struct sockaddr*)&sa,
                         sizeof(sa))) != 0) {
      strstream errMsg;
      errMsg << "UdpStack<" << getLclName() << " " << getRmtName()
             << ">::UdpStack error during socket connect: "
             << VSTRERROR << ends;
      
      cpLog(LOG_ERR,  errMsg.str());
   }   
   else {
      // Bind to the local interface, if specified, and only if we have not already
      // bound.  This almost definately will NOT work with IP-v6 as it currently
      // is implemented.
      if (desiredLocalIp.size() && !boundLocal) {
         uint32 lip;
         if (vtoIpString(desiredLocalIp.c_str(), lip) < 0) {
            strstream errMsg;
            errMsg << "UdpStack<" << getLclName() << " " << getRmtName()
                   << ">::UdpStack could not resolve host: "
                   << desiredLocalIp << ": " << VSTRERROR << ends;
            cpLog(LOG_ERR,  errMsg.str());
         }//if
         else {

#ifdef __linux__
            if (localDev.size()) {
               // Bind to specific device.
               char dv[15 + 1];
               strncpy(dv, localDev.c_str(), 15);
               if (setsockopt(socketFd, SOL_SOCKET, SO_BINDTODEVICE,
                              dv, 15 + 1)) {
                  cpLog(LOG_ERR, "ERROR:  setsockopt (BINDTODEVICE), dev: %s  error: %s\n",
                        dv, strerror(errno));
               }
            }
#endif

            memset(&sa, 0, sizeof(sa));
            
            sa.sin_family = AF_INET;
            sa.sin_port   = INADDR_ANY; /* any */
            sa.sin_addr.s_addr = htonl(lip);
            
            // Got the IP, now bind locally.
            if (bind(socketFd, (struct sockaddr*)(&sa), sizeof(sa)) < 0) {
               strstream errMsg;
               errMsg << "UdpStack<" << getLclName() << " " << getRmtName()
                      << ">::UdpStack could not bind during connect, ip: "
                      << desiredLocalIp << ": " << VSTRERROR << ends;
               cpLog(LOG_ERR,  errMsg.str());
            }
            else {
               localAddr.setHostName(vtoStringIp(lip));
               boundLocal = true;
            }
         }
      }
   }
}// connectPorts



// After testing, this method currently is not working on Linux and Sun4
// possibily because the system are not supporting it.
// Now the work around is to call connect twice.

void
UdpStack::disconnectPorts()
{
    if ((mode == recvonly) || (mode == inactive))
    {
        cpLog(LOG_ERR, "The UdpStack is recvonly or inactive.");
        return ;
    }

    struct sockaddr dummyAddr;

    memset((char*) &dummyAddr, 0, sizeof(dummyAddr));
    dummyAddr.sa_family = AF_INET;

    int result;

    if ((result = connect(socketFd,
                          (struct sockaddr*) & dummyAddr,
                          sizeof(dummyAddr))) != 0)
    {
        strstream errMsg;
        errMsg << "UdpStack<" << getLclName() << " " << getRmtName()
               << ">::UdpStack error during socket connect: " << VSTRERROR << ends;

        cpLog(LOG_ERR, errMsg.str());
    }

    dummyAddr.sa_family = AF_UNSPEC;
    if ((result = connect(socketFd,
                          (struct sockaddr*) & dummyAddr,
                          sizeof(dummyAddr))) != 0)
    {
        strstream errMsg;
        errMsg << "UdpStack<" << getLclName() << " " << getRmtName()
               << ">::UdpStack error during socket connect: " << VSTRERROR << ends;

        cpLog(LOG_ERR, errMsg.str());
    }
}


/// set the default destination
void
UdpStack::setDestination ( const NetworkAddress* host ) {
    if ((mode == recvonly) || (mode == inactive))
    {
        cpLog(LOG_ERR, "The UdpStack is recvonly or inactive.");
        return ;
    }
    doClient(*host);
}

void
UdpStack::setDestination ( const char* host, int port )
{
    assert(host);
    NetworkAddress netAddress(host, port);

    setDestination (&netAddress);
}

int UdpStack::getRxPort() {
   return localAddr.getPort();
}

int UdpStack::getTxPort() {
   return remoteAddr.getPort();
}


int UdpStack::getSocketFD () {
    return socketFd;
}

int UdpStack::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     int& maxdesc, uint64& timeout, uint64 now) {
   addToFdSet(input_fds);
   addToFdSet(exc_fds);
   if (sendBacklog.size()) {
      addToFdSet(output_fds);
   }
   if (socketFd > maxdesc) {
      maxdesc = socketFd;
   }
   return 0;
}

void
UdpStack::addToFdSet ( fd_set* set ) {
    FD_SET(socketFd, set);
}


int
UdpStack::getMaxFD ( int prevMax ) {
    return ( getSocketFD() > prevMax ) ? getSocketFD() : prevMax;
}


bool
UdpStack::checkIfSet ( fd_set* set ) {
    return ( FD_ISSET(socketFd, set) ? true : false );
}


int
UdpStack::receive ( const char* buf, const int bufSize, int flags ) {
   if ((mode == sendonly) || (mode == inactive)) {
      cpLog(LOG_ERR, "The stack is not in a state capable of receiving.");
      return -1;
   }

   int len = recv(socketFd,
                  (char *)buf, bufSize,
                  flags);
   if ( len < 0 ) {
      int err = ERRNO;
      if ((err == EAGAIN) || (err == EINTR) || (err == WSAEWOULDBLOCK)) {
         rxbusy++;
         return 0;
      }
      else {
         cpLog(LOG_ERR, "UdpStack: receive error: %s", VSTRERROR);
      }
   }
   else if (len == 0) {
      // received no bytes
      cpLog(LOG_DEBUG, "did not receive any data");
   }
   else {
      numBytesReceived += len;
      numPacketsReceived += 1;
   }

   if ( (logFlag) && (len > 0) ) {
      strstream lenln1;
      lenln1 << ++rcvCount << " " << len << "\n" << char(0);
      in_log->write(lenln1.str(), strlen(lenln1.str()));
      in_log->write(buf, len);
      in_log->write(separator, 6);
      lenln1.freeze(false);
   }

   return len;
}


int UdpStack::receiveFrom ( char* buffer,
                            const int bufSize,
                            struct sockaddr_in* sender,
                            int flags) {
   if ((mode == sendonly) || (mode == inactive)) {
      cpLog(LOG_ERR, "The stack is not capable to receive. ");
      return -1;
   }

// 25/11/03 fpi
// WorkAround Win32

// #if defined(WIN32)

    /* WSAECONNRESET (10054) Connection reset by peer.
     * A existing connection was forcibly closed by the remote host. This
     * normally results if the peer application on the remote host is suddenly
     * stopped, the host is rebooted, or the remote host used a "hard close" (see
     * setsockopt for more information on the SO_LINGER option on the remote
     * socket.) This error may also result if a connection was broken due to
     * "keep-alive" activity detecting a failure while one or more operations are
     * in progress. Operations that were in progress fail with WSAENETRESET.
     * Subsequent operations fail with WSAECONNRESET. Fix suggested by Alexandr Kuzmin gin@nnov.stelt.ru.
     *

    int len = 0;
    do
    {
        len = recvfrom( data->socketFd,
                            (char *)buffer,
                            bufSize,
                            flags, // flags
                            (sockaddr*) &xSrc,
                            (socklen_t*) &srcLen);
    } while( (len == -1) && (WSAGetLastError() == WSAECONNRESET ) );
#else */   
    int len = 0;
    int frlen = sizeof(*sender);
    if (!sender) {
       frlen = 0;
    }
#ifdef WIN32
    do {
       frlen = sizeof(*sender);
       int* flp = &frlen;
       if (!sender) {
          frlen = 0;
          flp = NULL;
       }
       len = recvfrom( socketFd,
                       buffer,
                       bufSize,
                       flags,
                       (struct sockaddr*)(sender),
                       flp);
    } while( (len == -1) && (WSAGetLastError() == WSAECONNRESET ) );
       
#else

    len = recvfrom( socketFd,
                    (char *)buffer,
                    bufSize,
                    flags,
                    (struct sockaddr*)(sender),
                    &frlen);
#endif
    
    if ( len <= 0 ) {
       int err = ERRNO;
       if ((err == EAGAIN) || (err == EINTR) || (err == WSAEWOULDBLOCK)) {
          // This can be a normal case...
          len = 0;
       }
       else {
          strstream errMsg;
          errMsg << "UdpStack<" << getLclName() << ">::receive error : "
                 << VSTRERROR << "(" << err << ") fd: "
                 << socketFd << " sender: " << sender << " frlen: " << frlen
                 << " sizeof(*sender): " << sizeof(*sender) << " buffer: " << buffer
                 << " bufsize: " << bufSize
                 << endl << ends;
          cpLog(LOG_ERR, "%s", errMsg.str());
       }
    }
    else {
       numBytesReceived += len;
       numPacketsReceived += 1;
    }

    if ( (logFlag) && (len > 0) ) {
       strstream lenln2;
       lenln2 << ++rcvCount << " " << len << "\n" << char(0);
       in_log->write(lenln2.str(), strlen(lenln2.str()));
       in_log->write(buffer, len);
       in_log->write(separator, 6);
       lenln2.freeze(false);
    }

    return len;
}//receiveFrom


int
UdpStack::receiveTimeout ( char* buffer,
                           const int bufSize,
                           struct sockaddr_in* sender,
                           int sec,
                           int usec)
{
#ifndef __vxworks
   timeval tv;
   fd_set rset;
   int fd = getSocketFD();

// Make the coket non-blocking and then change it againb after the receivfrom
#ifndef WIN32
   int retVal;
   bool madeNonBlocking = false;
   if (blockingFlg) {
      madeNonBlocking = true;
      if (setModeBlocking(false) < 0)
         return -1;
   }
#else
   int retVal;
   unsigned long non_blocking = 1;
   if (ioctlsocket(fd, FIONBIO, &non_blocking))
      return -1;
#endif

   // select will return upon timeout, error or received message
   FD_ZERO(&rset);
   FD_SET(fd, &rset);
   tv.tv_sec = sec;
   tv.tv_usec = usec;
   retVal = select(fd + 1, &rset, NULL, NULL, &tv);
   // we don't care about no stinking error
   if (retVal <= 0)
      return retVal;


   retVal = receiveFrom( buffer,
                         bufSize,
                         sender);

   cpLog(LOG_DEBUG, "UdpStack::receiveTimeout, retVal: %d\n", retVal);

#ifndef WIN32
   if (madeNonBlocking) {
      if (setModeBlocking(true) < 0);
      return -1;
   }
#else
   non_blocking = 0;
   if (ioctlsocket(fd, FIONBIO, &non_blocking) != 0)
      return -1;
#endif

   return retVal;
#else

   cpLog(LOG_ERR, "UdpStack::receiveTimeout  * not defined in vxworks *\n");
   return -1;
#endif
}


string UdpStack::toString() {
   ostrstream rv;
   rv << "lclName: " << lclName << " rmtName: " << rmtName << " pktLossProb: "
      << packetLossProbability << " BytesRx: " << numBytesReceived
      << " PktsRx: " << numPacketsReceived << " BytesTx: " << numBytesTransmitted
      << " PktsTx: " << numPacketsTransmitted << " mode: " << mode
      << " localAddr: " << localAddr.toString()
      << " remoteAddr: " << remoteAddr.toString() << " desiredLocalIp: "
      << desiredLocalIp << " logFlag: " << logFlag << " socketFd: "
      << socketFd << " rcvCount: " << rcvCount << " sndCount: "
      << sndCount << " blockingFlg: " << blockingFlg << endl;
   return rv.str();
}


int UdpStack::flushBacklog() {
   int t = 0;
   while (sendBacklog.size()) {
      Sptr<ByteBuffer> bb = sendBacklog.front();
      if (bb->getNetworkAddress()) {
         t = doTransmitTo(bb->getBuf(), bb->getLength(), bb->getNetworkAddress());
      }
      else {
         t = doTransmit(bb->getBuf(), bb->getLength());
      }

      if (t == bb->getLength()) {
         // Success
         sendBacklog.pop_front();
         t++;
      }
      else if (t < 0) {
         // Fatal error, give up on this pkt
         drop_in_bklog++;
         sendBacklog.pop_front();
         t++;
      }
      else {
         // Must have been EAGAIN, return and queue will try again next call
         break;
      }
   }
   return t;
}

// uses send() which is better to get ICMP msg back
// Returns < 0 on error, number of bytes written or queued otherwise.
int
UdpStack::queueTransmit ( const char* buf, const int length ) {
   if ((mode == recvonly) || (mode == inactive)) {
      cpLog(LOG_ERR, "The stack is not capable to transmit. ");
      return -1;
   }

   assert(buf);
   assert(length > 0);

   if ( packetLossProbability > 0.0 ) {
      static bool randInit = false;
      if (!randInit) {
         randInit = true;
         long seed = vgetCurMs();
         srandom(seed);
      }

      double numerator( random() & 0x7FFFFFFF );
      double denominator( 0x7FFFFFFF );
      double prob = numerator / denominator;
      if ( prob < packetLossProbability ) {
         // ok - just drop this packet
         // We are lying on purpose.
         return length;
      }
   }

   if (sendBacklog.size()) {
      flushBacklog();
   }

   int rslt = -1;
   bool enqueue = false;
   if (sendBacklog.size()) {
      enqueue = true;
   }
   else {
      // Got space to push
      rslt = doTransmit(buf, length);
      if (rslt == length) {
         // Pkt is heading towards the wire..it's as good as we can do!
      }
      else {
         if (rslt == 0) {
            enqueue = true;
         }
         // else, drop ye pkt
      }
   }

   if (enqueue) {
      // Queue it up, network is busy it seems
      Sptr<ByteBuffer> bb = new ByteBuffer(buf, length, NULL);
      sendBacklog.push_back(bb);
      rslt = length;
   }
   return rslt;
}//transmit


int UdpStack::doTransmit(const char* buf, int ln) {

   int count = send(socketFd, (char *)buf, ln, 0 /* flags */ );

   if ( count != ln ) {
      int err = ERRNO;
      if ((err == EAGAIN) || (err == EINTR)) {
         count = 0;
      }
      else {
         ostrstream errMsg;
         errMsg << "UdpStack<" << getRmtName() << ">::transmit: "
                << VSTRERROR;
         cpLog(LOG_ERR, errMsg.str());
      }
   }
   else {
      numBytesTransmitted += count;
      numPacketsTransmitted += 1;
   }

   if ( (logFlag) && (count > 0) ) {
      strstream lenln3;
      lenln3 << ++sndCount << " " << count << char(0);
      out_log->write(lenln3.str(), strlen(lenln3.str()));
      lenln3.freeze(false);
      
      strstream rAddress1;
      rAddress1 << " " << getRmtName() << "\n" << char(0);
      out_log->write(rAddress1.str(), strlen(rAddress1.str()));
      rAddress1.freeze(false);

      out_log->write(buf, count);
      out_log->write(separator, 6);
   }
   return count;
}//doTransmit


// Returns number of bytes written, or -errno on error.
int UdpStack::queueTransmitTo ( const char* buffer,
                                const int length,
                                const NetworkAddress* dest ) {

   if ((mode == recvonly) || (mode == inactive)) {
      cpLog(LOG_ERR, "The stack is not capable to transmit. ");
      return -EINVAL;
   }

   assert(buffer);
   assert(length > 0);

   if (dest) {
      if (strcasecmp(dest->getIpName().c_str(), "0.0.0.0") == 0) {
         assert("IP address is 0.0.0.0 in queueTransmitTo" == "fatal");
      }
   }

   if (sendBacklog.size()) {
      flushBacklog();
   }

   int rslt = -1;
   bool enqueue = false;
   if (sendBacklog.size()) {
      enqueue = true;
   }
   else {
      // Got space to push
      rslt = doTransmitTo(buffer, length, dest);
      if (rslt == length) {
         // Pkt is heading towards the wire..it's as good as we can do!
      }
      else {
         if (rslt == 0) {
            enqueue = true;
         }
         // else, drop ye pkt
      }
   }

   if (enqueue) {
      // Queue it up, network is busy it seems
      Sptr<ByteBuffer> bb = new ByteBuffer(buffer, length, dest);
      sendBacklog.push_back(bb);
      rslt = length;
   }
   return rslt;
}


int UdpStack::doTransmitTo( const char* buffer,
                            const int length,
                            const NetworkAddress* dest ) {

   struct sockaddr_in dest_addr;
   memset(&dest_addr, '\0', sizeof(dest_addr));
   dest_addr.sin_family = AF_INET;
   dest_addr.sin_addr.s_addr = htonl(dest->getIp4Address());
   dest_addr.sin_port = htons(dest->getPort());

   int count = sendto( socketFd,
                       (char*)buffer,
                       length,
                       0 ,  // flags
                       (struct sockaddr*) &dest_addr,
                       sizeof(dest_addr));

   if ( count != length ) {
      int err = ERRNO;
      if ((err == EAGAIN) || (err == EINTR)) {
         count = 0;
      }
      else {
         ostrstream errMsg;
         errMsg << "UdpStack<" << getRmtName() << ">::transmitTo error\n"
                << toString() << "\n buffer: " << (void*)(buffer)
                << " length: " << length << " dest: " << dest->toString()
                << ", error: " << VSTRERROR << ends;
         cpLog(LOG_ERR, errMsg.str());
         return -err;
      }
   }
   else {
      numBytesTransmitted += count;
      numPacketsTransmitted += 1;
   }

   if ( (logFlag) && (count > 0) ) {
      strstream lenln4;
      lenln4 << ++sndCount << " " << count << char(0);
      out_log->write(lenln4.str(), strlen(lenln4.str()));
      lenln4.freeze(false);

      strstream rAddress2;
      rAddress2 << " " << getRmtName() << "\n" << char(0);
      out_log->write(rAddress2.str(), strlen(rAddress2.str()));
      rAddress2.freeze(false);

      out_log->write(buffer, count);
      out_log->write(separator, 6);
   }

   return count;
}//transmitTo


///
void
UdpStack::joinMulticastGroup ( NetworkAddress group,
                               NetworkAddress* iface,
                               int ifaceInexe ) {
   // Previously for win32 platforms this function did nothing
   // Now it does what it should do using the ip_mreq structure defined
   // in W32McastCfg.hxx. contact nismail@cisco.com
   //#ifndef WIN32

#if defined(__linux__)
    if(NetworkConfig::instance().getAddrFamily() == PF_INET)
    {
        cpLog(LOG_INFO, "Interface (%s) index (%d) joining multicast group (%s)",
                     iface->getIpName().c_str(), ifaceInexe,
                     group.getIpName().c_str());
        struct ip_mreqn mreqn;
        struct sockaddr_storage groupAddr;
        group.getSockAddr(&groupAddr);
        memcpy(&mreqn.imr_multiaddr,
               &(((struct sockaddr_in*)&groupAddr)->sin_addr),
               sizeof(struct in_addr));
        struct sockaddr_storage intfAddr;
        iface->getSockAddr(&intfAddr);
        memcpy(&mreqn.imr_address,
               &((struct sockaddr_in*)&intfAddr)->sin_addr,
               sizeof(struct in_addr));
        mreqn.imr_ifindex = ifaceInexe;

        int ret;
        ret = setsockopt (getSocketFD(),
                          IPPROTO_IP,
                          IP_ADD_MEMBERSHIP,
                          (char*) & mreqn,
                          sizeof(struct ip_mreqn));
        if(ret < 0) {
           cpLog(LOG_ERR, "Failed to join multicast group on interface %s, reason:%s", iface->getIpName().c_str(),
                 VSTRERROR);
        }
        else {
           cpLog(LOG_INFO, "Joined multi-cast group");
        }
    }
    else {
        //Join to multi-cast group
        struct ipv6_mreq mreq6;
        string mCastGroup("ff13::1");
        if(inet_pton(AF_INET6, mCastGroup.c_str(), &mreq6.ipv6mr_multiaddr) < 0)
        {
            cpLog(LOG_ERR, "Failed to get the address for multicast group %s", mCastGroup.c_str());
            return;
        }
        cpLog(LOG_INFO, "Interface (%s) index (%d) joining multicast group (%s)",
                     iface->getIpName().c_str(), ifaceInexe,
                     mCastGroup.c_str());
        if(ifaceInexe > 0)
        {
            mreq6.ipv6mr_interface = ifaceInexe;
        }
        else
        {
            mreq6.ipv6mr_interface = 0;
        }
        int ret;
        ret = setsockopt (getSocketFD(),
                          IPPROTO_IPV6,
                          IPV6_ADD_MEMBERSHIP,
                          (char*) & mreq6,
                          sizeof(mreq6));
        if(ret < 0) {
           cpLog(LOG_ERR, "Failed to join multicast group on interface %s, reason:%s", iface->getIpName().c_str(),
                 VSTRERROR);
        }
        else {
           cpLog(LOG_INFO, "Joined multi-cast group");
        }
    }
#else
struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = (group.getIp4Address());

    mreq.imr_interface.s_addr = (iface->getIp4Address());
    //    mreq.imr_ifindex = ifaceInexe;

    int ret;
    ret = setsockopt (getSocketFD(),
                      IPPROTO_IP,
                      IP_ADD_MEMBERSHIP,
                      (char*) & mreq,
                      sizeof(struct ip_mreq));
#endif
//#endif // !WIN32
}

///
void
UdpStack::leaveMulticastGroup( NetworkAddress group,
                               NetworkAddress* iface,
                               int ifaceInexe )
{
// Previously for win32 platforms this function did nothing
// Now it does what it should do using the ip_mreq structure defined
// in W32McastCfg.hxx. contact nismail@cisco.com//#ifndef WIN32
//#ifndef WIN32
#if defined(__linux__)
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = (group.getIp4Address());
    mreqn.imr_address.s_addr = (iface->getIp4Address());
    mreqn.imr_ifindex = ifaceInexe;

    setsockopt (getSocketFD(),
                IPPROTO_IP,
                IP_DROP_MEMBERSHIP,
                (char*)&mreqn,
                sizeof(struct ip_mreqn));
#else
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = (group.getIp4Address());
    mreq.imr_interface.s_addr = (iface->getIp4Address());
    //    mreq.imr_ifindex = ifaceInexe;

    setsockopt (getSocketFD(),
                IPPROTO_IP,
                IP_DROP_MEMBERSHIP,
                (char*)&mreq,
                sizeof(struct ip_mreq));
#endif
//#endif // !WIN32
}


UdpStack::~UdpStack()
{
   if (in_log) {
      in_log->close();
      delete in_log;
      in_log = NULL;
   }

   if (out_log) {
      out_log->close();
      delete out_log;
      out_log = NULL;
   }

   if (socketFd >= 0) {
      closesocket(socketFd);
      socketFd = -1;
   }
}


int UdpStack::getBytesReceived () const {
   return numBytesReceived;
}

int
UdpStack::getPacketsReceived () const
{
    return numPacketsReceived;
}

int
UdpStack::getBytesTransmitted () const
{
    return numBytesTransmitted;
}

int
UdpStack::getPacketsTransmitted () const
{
    return numPacketsTransmitted;
}

#if 0
int
UdpStack::recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
                         struct sockaddr *sa, socklen_t *salenptr, 
                         struct in6_pktinfo *pktp)
{
    struct msghdr   msg;
    struct iovec    iov[1];
    int             n;
    struct cmsghdr  *cmptr;
#ifndef CMSG_LEN
#define CMSG_LEN(size)    (sizeof (struct cmsghdr) + (size))
#endif
#ifndef CMSG_SPACE
#define CMSG_SPACE(size)  (sizeof (struct cmsghdr) + (size))
#endif
    union {
      struct cmsghdr        cm;
      char   control[CMSG_SPACE(sizeof(struct in6_addr)) +
                 CMSG_SPACE(sizeof(struct in6_pktinfo))];
    } control_un;
//#if defined (__sparc)
//    msg.msg_accrights = control_un.control;
//    msg.msg_accrightslen = sizeof(control_un.control);
//#else

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    msg.msg_flags = 0;
//#endif
    msg.msg_name = sa;
    msg.msg_namelen = *salenptr;
#if defined (__FreeBSD__) || defined(__sparc)
    iov[0].iov_base = reinterpret_cast<char *>(ptr);
#else
    iov[0].iov_base = ptr;
#endif
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

// 25/11/03 fpi
// WorkAround Win32
#ifdef WIN32
	 if ((n = recv(fd, reinterpret_cast<char *>(msg.msg_iov[0].iov_base),
						msg.msg_iov[0].iov_len,
						*flagsp)) < 0)
	 {
		 return(n);
	 }

#else
    if ( (n = recvmsg(fd, &msg, *flagsp)) < 0)
    {
        return(n);
    }
#endif

    *salenptr = msg.msg_namelen;    
    if (pktp)
       memset(pktp, 0, sizeof(struct in6_pktinfo)); 

    if(flagsp)
//#if defined (__sparc)
//       if (msg.msg_accrightslen < sizeof(struct cmsghdr) || pktp == NULL)
//    {
//        return(n);
//    }
//#else
       *flagsp = msg.msg_flags;                /* pass back results */

    if (msg.msg_controllen < sizeof(struct cmsghdr) ||
            (msg.msg_flags & MSG_CTRUNC) || pktp == NULL)
    {
        return(n);
    }
//#endif

// 25/11/03 fpi
// WorkAround Win32
#ifndef WIN32

#if defined(__sparc)
/* To maintain backward compatibility, alignment needs to be 8 on sparc. */
#ifndef  _CMSG_HDR_ALIGNMENT
#define _CMSG_HDR_ALIGNMENT     8
#endif
#endif
#ifndef  _CMSG_HDR_ALIGN
#define _CMSG_HDR_ALIGN(x)      (((uintptr_t)(x) + _CMSG_HDR_ALIGNMENT - 1) & \
                                    ~(_CMSG_HDR_ALIGNMENT - 1))
#endif

#ifndef _CMSG_DATA_ALIGNMENT
#define _CMSG_DATA_ALIGNMENT   (sizeof (int))
#endif

#ifndef  _CMSG_DATA_ALIGN
#define _CMSG_DATA_ALIGN(x)     (((uintptr_t)(x) + _CMSG_DATA_ALIGNMENT - 1) & \
                                  ~(_CMSG_DATA_ALIGNMENT - 1))
#endif

#ifndef CMSG_FIRSTHDR
//#define CMSG_FIRSTHDR(size)    (sizeof (struct cmsghdr) + (size))
#define CMSG_FIRSTHDR(m)        ((struct cmsghdr *)((m)->msg_accrights))
#endif

#ifndef  CMSG_DATA
#define CMSG_DATA(c)                                                    \
        ((unsigned char *)_CMSG_DATA_ALIGN((struct cmsghdr *)(c) + 1))
#endif

#ifndef CMSG_NXTHDR
//#define CMSG_NXTHDR(size)  (sizeof (struct cmsghdr) + (size))
#define CMSG_NXTHDR(m, c)                                               \
        ((((uintptr_t)_CMSG_HDR_ALIGN((char *)(c) +                     \
        ((struct cmsghdr *)(c))->cmsg_len) + sizeof (struct cmsghdr)) > \
        (((uintptr_t)((struct msghdr *)(m))->msg_accrights) +             \
        ((uintptr_t)((struct msghdr *)(m))->msg_accrightslen))) ?         \
        ((struct cmsghdr *)0) :                                         \
        ((struct cmsghdr *)_CMSG_HDR_ALIGN((char *)(c) +                \
            ((struct cmsghdr *)(c))->cmsg_len)))
#endif

// 25/11/03 fpi
// WorkAround Win32

#else		// #ifndef WIN32
	// I don't know if this works under windows.  this needs to be
	// checked for IPV6.0
	// it is a complete guess.

	// defined in WINCRYPT.H
	#ifdef CMSG_DATA
	#undef CMSG_DATA
	#endif

	#define _CMSG_DATA_ALIGNMENT   1
	#define _CMSG_DATA_ALIGN(x)     (((uintptr_t)(x) + _CMSG_DATA_ALIGNMENT - 1) & ~(_CMSG_DATA_ALIGNMENT - 1))
	#define CMSG_DATA(c) ((unsigned char *)_CMSG_DATA_ALIGN((struct cmsghdr *)(c) + 1))

	#define _CMSG_HDR_ALIGNMENT     1
	#define _CMSG_HDR_ALIGN(x)      (((uintptr_t)(x) +_CMSG_HDR_ALIGNMENT - 1) & \
                                    ~(_CMSG_HDR_ALIGNMENT - 1))

	#define CMSG_FIRSTHDR(m)        ((struct cmsghdr*)((m)->msg_control))

	#define CMSG_NXTHDR(m, c) \
        ((((uintptr_t)_CMSG_HDR_ALIGN((char *)(c) +                     \
        ((struct cmsghdr *)(c))->cmsg_len) + sizeof (struct cmsghdr)) > \
        (((uintptr_t)((struct msghdr *)(m))->msg_control) +             \
        ((uintptr_t)((struct msghdr *)(m))->msg_controllen))) ?         \
        ((struct cmsghdr *)0) :                                         \
        ((struct cmsghdr *)_CMSG_HDR_ALIGN((char *)(c) +                \
            ((struct cmsghdr *)(c))->cmsg_len)))
/* 
next header 
		
(
	(
		( 
			(uintptr_t)_CMSG_HDR_ALIGN
				( 
					(char *)(c) 
					+ ((struct cmsghdr *)(c))->cmsg_len
				) 
			+ sizeof (struct cmsghdr)
		) 
		> 
		(
			(
				(uintptr_t)((struct msghdr
*)(m))->msg_accrights
			) 
			+ ( (uintptr_t)((struct msghdr
*)(m))->msg_accrightslen)
		)
	) 
	?       
	((struct cmsghdr *)0) 
	:                                        
	(
		(struct cmsghdr *)_CMSG_HDR_ALIGN
			(
				(char *)(c) 
				+ ((struct cmsghdr *)(c))->cmsg_len
			)
	)
)

*/

#endif		// #ifndef WIN32

    for (cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL;
             cmptr = CMSG_NXTHDR(&msg, cmptr))
    {
        if (cmptr->cmsg_level == IPPROTO_IPV6 &&
            cmptr->cmsg_type == IPV6_PKTINFO) 
        {
            memcpy(&pktp->ipi6_addr, CMSG_DATA(cmptr),
                               sizeof(struct in6_addr));
            continue;
        }
        cpLog(LOG_DEBUG, "unknown ancillary data, len = %d, level = %d, type = %d",
               cmptr->cmsg_len, cmptr->cmsg_level, cmptr->cmsg_type);
    }
    return(n);
}

#endif

int UdpStack::setModeBlocking(bool flg){
   if (blockingFlg != flg) {
      blockingFlg = flg;
      doSyncBlockingMode();
   }
   return 0;
}

int UdpStack::doSyncBlockingMode() {
   int fd = getSocketFD();
   if (fd < 0) {
      return -1;
   }
   if (blockingFlg) {

      // 25/11/03 fpi
      // WorkAraound Win32
#ifndef WIN32
      int flags;
      if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
         cpLog(LOG_ERR, "Failed to get block flag, reason:%s", VSTRERROR);
         return -1;
      }
      flags &= ~O_NONBLOCK;
      if (fcntl(fd, F_SETFL, flags) < 0) {
         cpLog(LOG_ERR, "Failed to make socket blocking, reason:%s", VSTRERROR);
         return -1;
      }
#else
      unsigned long non_blocking = 0;
      if (ioctlsocket(fd, FIONBIO, &non_blocking)) {
         cpLog(LOG_ERR, "Failed to make socket blocking, reason:%s", VSTRERROR);
         return -1;
      }
#endif
   }
   else {
      int fd = getSocketFD();

#ifndef WIN32
      int flags;
      if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
         cpLog(LOG_ERR, "Failed to get block flag, reason:%s", VSTRERROR);
         return -1;
      }
      flags |= O_NONBLOCK;
      if (fcntl(fd, F_SETFL, flags) < 0) {
         cpLog(LOG_ERR, "Failed to make socket non-block, reason:%s", VSTRERROR);
         return -1;
      }
#else
      unsigned long non_blocking = 1;
      if (ioctlsocket(fd, FIONBIO, &non_blocking)) {
         cpLog(LOG_ERR, "Failed to make socket non-block, reason:%s", VSTRERROR);
         return -1;
      }
#endif
   }
   return 0;
}//doSyncBlockingMode

