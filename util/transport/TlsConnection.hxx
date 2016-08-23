#ifndef TLSCONNECTION_HXX_
#define TLSCONNECTION_HXX_

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

#include "Connection.hxx"

#ifdef VOCAL_HAS_OPENSSL
#include <openssl/ssl.h>
#endif

#include "Data.hxx"

#ifndef VOCAL_HAS_OPENSSL
typedef void SSL;
typedef void SSL_CTX;
typedef void SSL_METHOD;
#endif


/**
   A TLS based (rather than TCP-based) Connection object.  Network
   connections are represented by these objects.<p>

   <b>Client Usage</b><p>

   <pre>

   // we assume here that an open TCP Connection exists in conn

   TlsConnection&#42; myConnection = 0;

   if(TlsConnection::hasTls() && myUseTls)
   {
     // do the TLS stuff if you need to.
   
     myConnection = new TlsConnection(&#42;conn);
     int result = myConnection->initTlsClient();
     if(result > 0)
     {
       
       // at this point, TLS is ok, so conn gets silently replaced
       // with the TLS form of the connection.
       
       conn = myConnection;
     }
     else
     {
       // here, the TLS negotiation failed, so print an error.
       LocalScopeAllocator lo;
       Data tmp = myConnection->getErrMsg(-1);
       cpLog(LOG_DEBUG, "TLS failed: %s!", tmp.getData(lo));
     }

     // from here, you can use the regular Connection procedure.
     
   </pre>

   Please see provisioning/psLib/PSNetworkClient.cxx for details.<p>

   <b>Server Usage</b><p>

   we again assume that you have created a TcpServerSocket already.
   Instead of passing a regular Connection in to the accept(), you
   should pass a TlsConnection.  Assume that there is already a
   TcpServerSocket <i>sock</i> which exists:

   <pre>
     TlsConnection conn;
     sock.accept(conn);

     // try to do TLS negotiation
     // the certificate and key file need to be somewhere
     if(conn.initTlsServer("path/to/certs", "path/to/keys") <= 0)
     {
       cpLog(LOG_ERR, "failed to do TLS correctly");
     }
     else
     {
       // everything OK, so continue.
     }


*/

class TlsConnection : public Connection {
    public:
        /// initiate an empty TLS server connection.
        TlsConnection(bool blocking);

        /** 
         * Begin a TLS client connection over an existing, open, TCP
         * connection.  Really is just to give this a unique signature so
         * that you know to be careful with who owns the socket.
         */
        TlsConnection(Connection& other, bool really);

        virtual ~TlsConnection() throw (Vocal::SystemException);

        /// initialize as a TLS client
        int initTlsClient();

        /// initialize as a TLS server
        int initTlsServer(const char* certificate,
                          const char* key);

        /// get the SSL* object (from openssl)
        SSL* getSsl();

        /**
           Does this connection have TLS running at this moment.
           @return true if TLS active on this connection, false otherwise.
        */
        bool isTls() const;

        /** 
            Tell whether this object was built with TLS support.

            @return If not, return false.  if so, return true.
        */
        static bool hasTls();

        /** return the openssl error message.  this function be called
         * from the same thread as received the error.
         *
         * @param e    the error code which was returned by the call.
         */
        Data getErrMsg(int e);

        virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                          uint64 now);

        virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                           int& maxdesc, uint64& timeout, uint64 now);

    protected:
        virtual int iclose();
        virtual ssize_t iread();
        virtual ssize_t iwrite();

        SSL_CTX* ctx;
        SSL*     ssl;
        SSL_METHOD *meth; // Does not need freeing???
        Data myCertificate;
        Data myKey;

    private:
        /// assignment operator (Don't use)
        const TlsConnection& operator=(const TlsConnection& x);

};

#endif
