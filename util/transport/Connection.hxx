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
    "$Id: Connection.hxx,v 1.2 2004/05/04 07:31:16 greear Exp $";

#include "vin.h"
#include "global.h"
#include <string.h>
#include <BugCatcher.hxx>

#include <string>

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
#define MAXLINE   256

/**
   A stream based Connection object.  Network connections are
   represented by these objects.<p>

   <b>Usage</b>

   assume for the moment that you have a valid Connection named conn
   (see TcpClientSocket or TcpServerSocket for details on how to
   create a valid Connection).<p>

   <pre>
   
   // to write
   // note -- this does NOT write the trailing NULL
   conn.writeData("test\n", 5);
   
   // to read
   
   char buf[256];
   int n;

   // this reads a line as opposed to a certain number of bytes.
   int err = conn.readLine(buf, 255, n);
   
   </pre>

*/
class Connection: public BugCatcher
{
    public:

        /**
           Construct a connection.
           @param blocking  create a blocking object

        */
        Connection(bool blocking = true);

        /**
           Construct a connection.
           @param conId  file descriptor to wrap with connection object.
           @param blocking  create a blocking object.
        */
        Connection(int conId, bool blocking = true);

        /** return the file descriptor */
        inline int getConnId() const
        {
            return _connId;
        }

        /** return the address connecting to */
        inline struct sockaddr& getConnAddr()
        {
            return *_connAddr;
        }
 
        /** return length of the connecting address. */
        inline socklen_t getConnAddrLen() const
        {
            return _connAddrLen;
        }

        Connection(const Connection& other);

        virtual ~Connection();

        Connection& operator=(const Connection& other);

        /**
           this is true if the two Connection objects have the same
           fd, false otherwise.
        */

        bool operator==(const Connection& other)
        {
            return (_connId == other._connId);
        }

        /**
           this is false if the two Connection objects have the same
           fd, true otherwise.
        */

        bool operator!=(const Connection& other)
        {
            return (_connId != other._connId);
        }

        /**
           Reads line until '\n' is encountered or data ends.

           @param data buffer to read data into
           @param maxlen  maximum number of bytes to read into buffer
           @param bytesRead  set to number of bytes read.

           @return 0 if no more data, or number of bytes read, or -1
           if connection is already closed

           @throw VNetworkException
         */
        int readLine(void* data, size_t maxlen, int &bytesRead) throw (VNetworkException&);

        /**
           Reads bytes from the connection.

           @param data buffer to read data into
           @param nchar  maximum number of bytes to read into buffer
           @param bytesRead  set to number of bytes read.

           @return 0 if no more data, or number of bytes read, or -1
           if connection is closed

           @throw VNetworkException
         */
        int readn(void* data, size_t nchar, int &bytesRead) throw (VNetworkException&);



        /**
           Reads bytes from the connection.

           @param data buffer to read data into
           @param nchar  maximum number of bytes to read into buffer

           @return 0 if no more data, or number of bytes read, or -1
           if connection is closed

           @throw VNetworkException
         */
        int readn(void *data, size_t nchar) throw (VNetworkException&);


        /** 
            Writes bytes to the connection.

            @param data buffer to write data from
            @param n  number of bytes to write.

            @throw VNetworkException
        */
        void writeData(void* data, size_t n) throw (VNetworkException&);


        /** 
            Writes bytes to the connection.

            @param string string to write to the network.

            @throw VNetworkException
        */
        void writeData(string& data) throw (VNetworkException&);


        /** 
            Gets the connection description.

            @return connection description (far end) in the format
            IP_ADDRESSS:Port.
        */
        string getDescription() const;

        /** 
            Gets the IP of the destination.
            
            @return IP of the destination.
        */
        string getIp() const;

        ///Gets the port of the destination 
        int getPort() const;


        /// Still connected?  true if so.
        bool isLive() const
        {
            return (_live);
        }

        /// close connection.
        int close();


        /** 
            Check if data is ready to be read.

            @param seconds number of seconds to wait for data to be read.
            @param mSeconds number of microseconds to wait for data to be read.

            @return true if data is ready to be read, or false otherwise.
        */
        bool isReadReady(int seconds = 0, int mSeconds = 20000) const;

        /// initialize the SIGPIPE signal handler (for broken pipes)
        void initialize();

        /// handler for SIGPIPE signal
        static void signalHandler(int signo);

        /// @deprecated
        void deepCopy(const Connection& src, char** bufPtr, int* bufLenPtr);

    protected:
        ssize_t effRead(char* ptr);

        virtual int iclose();
        virtual ssize_t iread(char* buf, size_t count);
        virtual ssize_t iwrite(char* buf, size_t count);

        /**Sets the connection state to be blocking or non-blocking
           based on the type of the connection.
         */
        void setState();

        friend class TcpServerSocket;
        friend class TcpClientSocket;
        int _connId;
        bool _live;
        socklen_t _connAddrLen;
        struct sockaddr* _connAddr;
        bool _blocking;
        static bool _init;  /// Set to true if signal handler initialized
        bool _isClient; //  set if it is the client
};


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif
