#ifndef CONNECTION_HXX_
#define CONNECTION_HXX_

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

static const char* const ConnectionHeaderVersion =
    "$Id: Connection.hxx,v 1.7 2004/06/03 07:28:15 greear Exp $";

#include "vin.h"
#include "global.h"
#include <string.h>
#include <BugCatcher.hxx>
#include <IOBuffer.hxx>
#include <string>
#include <misc.hxx>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

#if defined(__svr4__) || defined(__SUNPRO_CC)

#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#endif

#ifdef __APPLE__
typedef int socklen_t;
#include <netinet/in.h>
#endif

#include "VNetworkException.hxx"

// 25/11/03 fpi
// WorkAround Win32
#ifdef _WIN32
#define snprintf _snprintf
#endif

// 16/1/03 fpi
// WorkAround Win32
#ifdef WIN32
extern char*  inet_ntop(int af, const void *src, char *dst, size_t size);
#endif

class TcpServerSocket;
class TcpClientSocket;

typedef struct sockaddr SA;

// TODO:  This could go elsewhere, as is does not directly relate to this class.
#define MAXLINE 256


class Connection: public BugCatcher {
public:
        
   /**
      Construct a connection.
      @param blocking  create a blocking object, or non-blocking if false.

   */
   Connection(bool blocking);

   /**
      Construct a connection.
      @param conId  file descriptor to wrap with connection object.
      @param blocking  create a blocking object.
   */
   Connection(int conId, bool blocking);


   /** On_purpose is just to make sure we call this method
    * on purpose, and thus are paying atention to making sure
    * the underlying socket descriptors are set correctly.
    */
   Connection(const Connection& other, bool on_purpose);

   /** return the file descriptor */
   int getConnId() const {
      return _connId;
   }

   /** return the address connecting to */
   struct sockaddr& getConnAddr() {
      return *_connAddr;
   }
 
   /** return length of the connecting address. */
   socklen_t getConnAddrLen() const {
      return _connAddrLen;
   }

   virtual ~Connection();

   // Be very careful with this.  You must set closeOnDestruct
   // correctly!
   Connection& operator=(const Connection& other);

   virtual void setCloseOnDestruct(bool b) { closeOnDestruct = b; }
   virtual bool shouldCloseOnDestruct() const { return closeOnDestruct; }

   virtual bool needsToWrite() { return outBuf.getCurLen() > 0; }
   virtual int getSendQueueSize() { return outBuf.getCurLen(); }
   virtual void consumeRcvdBytes(int cnt) { rcvBuf.dropFromTail(cnt); }

   // Returns the number of bytes actually peeked, and
   // buf will be null-terminated.
   int peekRcvdBytes(unsigned char* buf, int mx_buf_len);

   /**
      this is true if the two Connection objects have the same
      fd, false otherwise.
   */

   bool operator==(const Connection& other) {
      return (_connId == other._connId);
   }

   /**
      this is false if the two Connection objects have the same
      fd, true otherwise.
   */

   bool operator!=(const Connection& other) {
      return (_connId != other._connId);
   }


   /** Returns >= 0 if we received a newline, OR if the entire buffer is full
    * but there is no newline.  RsltBuf will be null terminated.  Maxlen specifies
    * the memory allocated for rsltBuf and is uses to ensure we do not over-run
    * the buffer.
    * In general, call this method repeatedly untill it returns < 0
    */
   int getLine(char* rsltBuf, int maxlen);


   /** 
       Gets the connection description.
       @return connection description (far end) in the format
       IP_ADDRESSS:Port.
   */
   string getDescription() const;

   /** 
       Gets the IP of the peer machine (destination).
       @return IP of the destination.
   */
   string getPeerIp() const;

   //Gets the port of the destination 
   int getPeerPort() const;

   // Still connected?  true if so.
   bool isLive() const {
      return (_connId >= 0);
   }

   // close connection.
   int close();

   // initialize the SIGPIPE signal handler (for broken pipes)
   void initialize();

   // handler for SIGPIPE signal
   static void signalHandler(int signo);

   void setConnectInProgress(bool v) { _inProgress = true; }
   bool isConnectInProgress() const { return _inProgress; }

   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);


   // Get the file descriptor for the socket - use with care or not at all
   int getSocketFD () { return getConnId(); }

   // Add this stacks file descriptors to the the fdSet
   void addToFdSet ( fd_set* set );

   // Find the max of any file descripts in this stack and the passed value
   int getMaxFD ( int prevMax);

   // Check and see if this stacks file descriptor is set in fd_set
   bool checkIfSet ( fd_set* set );

   bool isBlocking() { return _blocking; }

   virtual int read();

   virtual int write();

   // Queue it for send.  It will be written as soon as possible.
   virtual int queueSendData(const char* data, int len);
   virtual int queueSendData(const string& d) {
      return queueSendData(d.c_str(), d.size());
   }

   // Clear out our buffers, does not attempt to flush.
   virtual void clear();

protected:

   /**Sets the connection state to be blocking or non-blocking
      based on the type of the connection.
   */
   void setState();

   virtual int iclose();
   virtual int iread();
   virtual int iwrite();

   friend class TcpServerSocket;
   friend class TcpClientSocket;
   int _connId;
   socklen_t _connAddrLen;
   struct sockaddr* _connAddr;
   bool _blocking;
   bool _inProgress; // Doing a non-blocking connect
   static bool _init;  // Set to true if signal handler initialized
   bool _isClient; //  set if it is the client
   bool closeOnDestruct; /* Should we close our socket on destruct, or not.
                          * Helps with shared sockets. */

   IOBuffer outBuf;
   IOBuffer rcvBuf;

private:
   // Don't use this, makes it a total PITA to figure out how to
   // safely close file descriptors.
   Connection(const Connection& other);
};

#endif
