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

static const char* const RtspTcpConnection_cxx_version =
    "$Id: RtspTcpConnection.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

const int KEEP_ALIVE_COUNTER = 10;

#include "RtspTcpConnection.hxx"

#include <sys/types.h>
#include <sys/wait.h>


RtspTcpConnection::TcpConnectionMap RtspTcpConnection::myTcpConnectionMap;


RtspTcpConnection::RtspTcpConnection( Fifo< Sptr<RtspMsg> >& recvFifo, int listenPort )
    : myTcpStack( listenPort ),
      myMsgFifo( recvFifo ),
      myShutdown( false )
{
    // setup signal handlers for TCP connections
    signal( SIGTERM, RtspTcpConnection::sigTerm );
    //signal( SIGINT, RtspTcpConnection::sigTerm );
    //signal( SIGHUP, RtspTcpConnection::sigTerm );
    //signal( SIGQUIT, RtspTcpConnection::sigTerm );

    myRecvTcpThread.spawn( recvTcpThreadWrapper, this );
}


RtspTcpConnection::~RtspTcpConnection()
{
    cpLog( LOG_DEBUG, "~RtspTcpConnection" );
    destory();
}


void
RtspTcpConnection::destory()
{
    // close server tcp connection
    if( myTcpStack.getServerConn().isLive() )
    {
        closeTcpConnection();
    }

    // close all tcp connections
    while( !myTcpConnectionMap.empty() )
    {
        Sptr<RtspTcpBuffer> tcpBufferObj = (*myTcpConnectionMap.begin()).second;
        tcpBufferObj->closeConnection();
        myTcpConnectionMap.erase( tcpBufferObj->getMapIndex() );
    }
}


void
RtspTcpConnection::sigTerm( int signo )
{
    cpLog( LOG_ALERT, "Signal term caught" );
    //destory();
}


void
RtspTcpConnection::closeTcpConnection()
{
    cpLog( LOG_DEBUG, "Closing server tcp connection" );

    // stop recvTcpThread
    myShutdownMutex.lock();
    myShutdown = true;
    myShutdownMutex.unlock();

    // wait until receive thread completed
    myRecvTcpThread.join();

    // close server tcp connection
    myTcpStack.close();
}


void*
RtspTcpConnection::recvTcpThreadWrapper( void* p )
{
    return static_cast<RtspTcpConnection*>(p)->recvTcpThread();
}


void*
RtspTcpConnection::recvTcpThread()
{
    cpLog( LOG_DEBUG, "RtspTcpConnection thread is running" );

    int maxFd;
    fd_set baseReadFd;
    struct timeval timeout;
    int selectResult;

    FD_ZERO( &baseReadFd );
    FD_SET( myTcpStack.getServerConn().getConnId(), &baseReadFd );
    maxFd = myTcpStack.getServerConn().getConnId();


    // process all incoming TCP messages
    while( 1 )
    {
        // setup select parameters
        fd_set readFd = baseReadFd;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        
        selectResult = select( maxFd+1, &readFd, 0, 0, &timeout );
        if( cpLogGetPriority() >= LOG_DEBUG_HB )
        {
            cerr<<"tcp";
        }
        if( selectResult < 0 )
        {
            cpLog( LOG_DEBUG, "select() error in recvTcpThread" );
        }
        else if( selectResult == 0 )
        {
            // check if stop accepting new network connections
            myShutdownMutex.lock();
            bool shutdownNow = myShutdown;
            myShutdownMutex.unlock();
            if( shutdownNow )
            {
                break;
            }
            else
            {
                continue;
            }
        }

        // check for new connections
        if( FD_ISSET( myTcpStack.getServerConn().getConnId(), &readFd ) )
        {
            // accept new connection
            bool successAccept = true;
            Connection conn;
            try
            {
                myTcpStack.accept( conn );
            }
            catch( VException& e )
            {
                cpLog( LOG_DEBUG, e.getDescription().c_str() );
                successAccept = false;
            }

            if( successAccept )
            {
                // create new session
                Sptr<RtspTcpBuffer> newTcpBufferObj= new RtspTcpBuffer( conn,
                                    KEEP_ALIVE_COUNTER, myMsgFifo );

                // added new session to myTcpConnectionMap
                cpLog( LOG_DEBUG, "Adding %s to myTcpConnectionMap",
                       newTcpBufferObj->getDescription().c_str() );
                myTcpConnectionMap[ newTcpBufferObj->getMapIndex() ] =
                                    newTcpBufferObj;

                // add to select map
                int fd = conn.getConnId();
                FD_SET( fd, &baseReadFd );
                if( fd > maxFd )  maxFd = fd;
            }
        }

        // process some connections
        for( TcpConnectionMap::iterator itr = myTcpConnectionMap.begin();
             itr != myTcpConnectionMap.end(); )
        {
            Sptr<RtspTcpBuffer> tcpBufferObj = (*itr).second;
            assert(tcpBufferObj != 0);
            Connection conn = tcpBufferObj->getConnection();
            if( FD_ISSET( conn.getConnId(), &readFd ) )
            {
                if( !tcpBufferObj->processConnection( tcpBufferObj ) )
                {
                    // connection failure, delete from map
                    cpLog( LOG_DEBUG,
                           "Tcp connnection closed, deleting %s",
                           tcpBufferObj->getDescription().c_str() );

                    // delete from select map
                    FD_CLR( conn.getConnId(), &baseReadFd );

                    conn.close();
                    myTcpConnectionMap.erase( itr++ );
                }
                else
                {
                    ++itr;
                }
            }
            else
            {
                ++itr;
            }
        }
    }

    myRecvTcpThread.exit();
    return 0;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
