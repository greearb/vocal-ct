#ifndef RtspTcpConnection_Hxx
#define RtspTcpConnection_Hxx
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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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


static const char* const RtspTcpConncetion_hxx_Version =
    "$Id: RtspTcpConnection.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspTcpBuffer.hxx"

#include <map>
#include <signal.h>

#include "VMutex.h"
#include "VThread.h"
#include "Tcp_ServerSocket.hxx"


/** RtspTcpConnection
 *  Accepts a new TCP connection as a new RtspTcpBuffer object
 *  Addes it to the connection map and select() processing TCP traffic
 */
class RtspTcpConnection
{
    public:
        /** constructor
         *  @param recvFifo fifo to insert new RTSP messages
         *  @param listenPort port to accept new connections
         */
        RtspTcpConnection( Fifo< Sptr<RtspMsg> >& sharedRecvFifo,
                           int listenPort );
        /** deconstructor */
        virtual ~RtspTcpConnection();

        /** accept new tcp connections and processes them */
        void* recvTcpThread();
        /** recvTcpThread wrapper */
        static void* recvTcpThreadWrapper( void* p );

        /** stops recvThread and closes server tcp conneciton */
        void closeTcpConnection();

        /** signal handler for SIGTERM */
        static void sigTerm( int signo );
        /** signal handler for SIGCHLD cleanup the child */
        static void sigChld( int signo );
        /** signal handler for SIGABRT */
        static void sigAbrt( int signo );

    private:
        /** closes all connections and server connection */
        void destory();

        /** TcpConnection map type 
         *  Used to store connected tcp connection
         *  Map indexed on RtspTcpBuffer.myMapId (connId)
         */
        typedef map< int, Sptr<RtspTcpBuffer> > TcpConnectionMap;
        /** tcp connection map */
        static TcpConnectionMap myTcpConnectionMap;

        /** tcp stack */
        TcpServerSocket myTcpStack;

        /** RtspServer receiving fifo */
        Fifo< Sptr<RtspMsg> >& myMsgFifo;

        /** thread for accepting new TCP connections */
        VThread myRecvTcpThread;
        /** */
        VMutex myShutdownMutex;
        /** */
        bool myShutdown;

    protected:
        /** suppress copy constructor */
        RtspTcpConnection( const RtspTcpConnection& );
        /** suppress assignment operator */
        RtspTcpConnection& operator=( const RtspTcpConnection& );
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
