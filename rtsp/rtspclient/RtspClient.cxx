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

static const char* const RtspClient_cxx_version =
    "$Id: RtspClient.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <list>

#include <fcntl.h>

#include "NetworkAddress.h"
#include "Connection.hxx"
#include "Tcp_ClientSocket.hxx"
#include "VNetworkException.hxx"
#include "cpLog.h"

#include "RtspTransceiver.hxx"
#include "RtspMsg.hxx"
#include "RtspMsgParser.hxx"

int writeData( Connection conn, const char* buffer, int len );
int readData( Connection conn, char* buffer, size_t bytesToRead);

int main( int argc, char* argv[] )
{
    cpLogSetPriority(LOG_DEBUG_STACK);

    if( argc < 3 )
    {
        printf("Usage: %s remotehost remoteport\n", argv[0] );
        return 0;
    }

    // build remote address
    string rmtHost = argv[1];
    int rmtPort = atoi( argv[2] );
    NetworkAddress remoteAddress( rmtHost, rmtPort );

    // create TCP connection
    // close connection = true
    // blocking = false
    cout <<"Connecting to: ";
    remoteAddress.print( cout );
    cout <<endl;
    TcpClientSocket cSock( remoteAddress, true, false );
    try
    {
        cSock.connect();
    }
    catch (VException &e)
    {
        cerr <<e.getDescription().c_str()<<endl;
        return 0;
    }

    // create a buffer
    list< pair<int, string> > myBufferList;
    myBufferList.push_back( pair<int,string>( 0, "DESCRIBE rtsp://www.vovida.com/sounds/LINEAR16 RTSP/1.0\r\nCSeq: 1\r\nAccept: application/sdp\r\n\r\n" ) );

    myBufferList.push_back( pair<int,string>( 0, "SETUP rtsp://www.vovida.com/sounds/LINEAR16.raw RTSP/1.0\r\nCSeq: 2\r\nTransport: rtp/avp;unicast;client_port=9000-9001\r\n\r\n" ) );
    myBufferList.push_back( pair<int,string>( 0, "PLAY rtsp://www.vovida.com/sounds/LINEAR16.raw RTSP/1.0\r\nSession: 6000\r\nCSeq: 3\r\n\r\n" ) );

    myBufferList.push_back( pair<int,string>( 5, "SETUP rtsp://www.vovida.com/sounds/LINEAR16 RTSP/1.0\r\nCSeq: 2\r\nTransport: rtp/avp;unicast;client_port=9000-9001\r\n\r\n" ) );
    myBufferList.push_back( pair<int,string>( 0, "PLAY rtsp://www.vovida.com/sounds/LINEAR16 RTSP/1.0\r\nSession: 6001\r\nCSeq: 3\r\n\r\n" ) );

    myBufferList.push_back( pair<int,string>( 10, "TEARDOWN rtsp://www.vovida.com/sounds/LINEAR16 RTSP/1.0\r\nSession: 6001\r\nCSeq: 6\r\n\r\n" ) );
    myBufferList.push_back( pair<int,string>( 10, "TEARDOWN rtsp://www.vovida.com/sounds/LINEAR16 RTSP/1.0\r\nSession: 6000\r\nCSeq: 6\r\n\r\n" ) );


    char buffer[1024];
    Sptr< RtspMsg > savedMsg = 0;
    Sptr< RtspMsg > response = 0;
    for( list< pair<int, string> >::iterator itr = myBufferList.begin();
         itr != myBufferList.end(); itr++ )
    {
        if( itr->first != 0 )
        {
            cout <<"Waiting "<<itr->first<<" seconds until next request\n";
            sleep(itr->first);
        }
        string msg = itr->second;
        writeData( cSock.getConn(), msg.c_str(), msg.length() );

        if (cSock.getConn().isReadReady(1)) 
        {
            int cc = readData( cSock.getConn(), buffer, 1024 );
            int ccout = 0;

            cout << "cc= " << cc << endl;
            cout << "ccout= " << ccout << endl;
            RtspMsgParser parser;
            // only try to get one msg from this buffer
            response = parser.preParse( buffer, cc, ccout, savedMsg );

            if( response->getCompletionFlag() == RTSP_MSG_COMPLETE )
            {
                savedMsg = 0;
            } 
            else
            {
                savedMsg = response;
                if (cc < 1024)
                    cout << "Response msg is fragmented." << endl;
                else
                    cout << "Response msg is too big." << endl;
            }
        }
    }

    cout <<"Sleeping 5 sec\n";
    sleep( 5 );

    cout <<"Closing connection\n";
    cSock.close();

    cout <<"Done\n";
    return 0;
}

int
writeData( Connection conn, const char* buffer, int len )
{
    cout <<"Sending\n";
    cout <<"---------------------\n";
    cout <<buffer<<endl;
    cout <<"---------------------\n";

    // write buffer to TCP connection
    try
    {
        conn.writeData( (void*)buffer, len );
    }
    catch( VNetworkException& e )
    {
        cerr <<e.getDescription()<<endl;
        return 0;
    }
    return 0;
}


int
readData( Connection conn, char* rdbuf, size_t bytesToRead )
{
    int bytesRead;
    RtspTransceiver::readData( conn, (void*)rdbuf, bytesToRead, bytesRead);

    if (bytesRead > 0)
    {
        Data dRead(rdbuf, bytesRead);
        cout <<"Server responded: \n";
        cout <<"---------------------\n";
        cout <<"bytesRead= " << bytesRead << endl;
        cout <<dRead.getData()<<endl;
        cout <<"---------------------\n";
        return bytesRead;
    }
    else
    {
        cout << "Server no response \n";
        return 0;
    }
}

/*
int
readData( Connection conn, char* rdbuf )
{
    // read reply from TCP connection
    string dRead;
    int nRead = 0;
    cout <<"Waiting for reply\n";
    fcntl(conn.getConnId(), F_SETFL, O_NONBLOCK);
    while( conn.readLine( rdbuf, (sizeof(rdbuf) - 1), nRead ) )
    {
        rdbuf[nRead] = '\0';
        dRead += rdbuf;
        rdbuf[0] = '\0';
        nRead = 0;
        if( !conn.isReadReady() )
            break;
    }
    rdbuf[nRead] = '\0';
    dRead += rdbuf;
    cout <<"Server responded: \n";;
    cout <<"---------------------\n";
    cout <<dRead<<endl;
    cout <<"---------------------\n";
    return 0;
}
*/
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
