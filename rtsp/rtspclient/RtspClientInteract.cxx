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

static const char* const RtspClientInteract_cxx_version =
    "$Id: RtspClientInteract.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


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

#include "RtspMsg.hxx"
#include "RtspMsgParser.hxx"
#include "RtspTransceiver.hxx"

#include "RtpSession.hxx"


int writeData( Connection conn, char* buffer, int len );
int readData( Connection conn, char* buffer, size_t len);

int main( int argc, char* argv[] )
{
    cpLogSetPriority(LOG_DEBUG);

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

    char buffer[1024];
    char choice[10];

    int sessionId;
    int cseqCounter = 0;
    int cRand = 0;

    bool quit = false;
    Sptr< RtspMsg > savedMsg = 0;
    Sptr< RtspMsg > response = 0;
    while( !quit )
    {
        memset( buffer, 0, 1024);
        cout<<"Enter command (d, a, s, w(setup with record), p, r, o(pause), t)" << endl;
        cin >> choice;
        cin >> sessionId;
        cout<<endl;

        switch( choice[0] )
        {
            case 'd':
            {
                sprintf( buffer,"DESCRIBE rtsp://www.vovida.com/sounds/basic RTSP/1.0\r\nCSeq: %d\r\nAccept: application/sdp\r\n\r\n",
                         cseqCounter++ );
                break;
            }
            case 'b':
            {
                sprintf( buffer,"DESCRIBE rtsp://www.vovida.com/sounds/wav728.wav RTSP/1.0\r\nCSeq: %d\r\nAccept: application/sdp\r\n\r\n",
                         cseqCounter++ );
                break;
            }
            case 'a':
            {
                sprintf( buffer,"ANNOUNCE rtsp://www.vovida.com/sounds/basic RTSP/1.0\r\nCSeq: %d\r\nContent-Length: 109\r\nContent-Type: application/sdp\r\n\r\nv=0\r\no=- 12345 12345 IN IP4 0.0.0.0\r\ns=Vovida RTSP Server\r\nt=0 0\r\nm=audio 0 RTP/AVP 0\r\na=rtpmap:0 PCMU/8000\r\n",
                         cseqCounter++ );
                break;
            }
            case 'm':
            {
                sprintf( buffer,"ANNOUNCE rtsp://www.vovida.com/sounds/basic RTSP/1.0\r\nCSeq: %d\r\nContent-Length: 21\r\nContent-Type: application/sdp\r\n\r\nm=audio 0 RTP/AVP 0\r\n",
                         cseqCounter++ );
                break;
            }
            case 'x':   // annouce with incremental filename
            {
                cRand = cseqCounter;
                sprintf( buffer,"ANNOUNCE rtsp://www.vovida.com/sounds/basic%d.au RTSP/1.0\r\nCSeq: %d\r\nContent-Length: 109\r\nContent-Type: application/sdp\r\n\r\nv=0\r\no=- 12345 12345 IN IP4 0.0.0.0\r\ns=Vovida RTSP Server\r\nt=0 0\r\nm=audio 0 RTP/AVP 0\r\na=rtpmap:0 L16M/8000\r\n",
                         getpid(), cseqCounter++ );
                break;
            }
            case 'y':  //performance play test (incremental client port)
            {
                sprintf( buffer,"SETUP rtsp://www.vovida.com/sounds/basic%d.au RTSP/1.0\r\nCSeq: %d\r\nTransport: rtp/avp;unicast;client_port=%d-%d;mode=\"record\"\r\n\r\n",
                         getpid(), cseqCounter++, cRand+9000, cRand+9000+1 );
                cRand++;
                break;
            }
            case 's':
            {
                sprintf( buffer,"SETUP rtsp://www.vovida.com/sounds/basic RTSP/1.0\r\nCSeq: %d\r\nTransport: rtp/avp;unicast;client_port=9000-9001\r\n\r\n",
                         cseqCounter++ );
                break;
            }
            case 'q':  //setup for PCMU
            {
                sprintf( buffer,"SETUP rtsp://www.vovida.com/sounds/LINEAR16-PCMU.raw RTSP/1.0\r\nCSeq: %d\r\nTransport: rtp/avp;unicast;client_port=9000-9001\r\n\r\n",
                         cseqCounter++ );
                break;
            }
            case 'w':  //setup(record)
            {
                sprintf( buffer,"SETUP rtsp://www.vovida.com/sounds/basic RTSP/1.0\r\nCSeq: %d\r\nTransport: rtp/avp;unicast;client_port=9000-9001;mode=\"record\"\r\n\r\n",
                         cseqCounter++ );
                break;
            }
            case 'n':  //setup(record)
            {
                sprintf( buffer,"SETUP rtsp://www.vovida.com/sounds/basic2 RTSP/1.0\r\nCSeq: %d\r\nTransport: rtp/avp;unicast;client_port=9000-9001;mode=\"record\"\r\n\r\n",
                         cseqCounter++ );
                break;
            }
            case 'g':  //setup(play)
            {
                sprintf( buffer,"SETUP rtsp://www.vovida.com/sounds/twister-PCMU.au RTSP/1.0\r\nCSeq: %d\r\nTransport: rtp/avp;unicast;client_port=9000-9001;mode=\"PLAY\"\r\n\r\n",
                         cseqCounter++ );
                break;
            }
            case 'h':  //setup(play)
            {
                sprintf( buffer,"SETUP rtsp://www.vovida.com/sounds/sample-sound/au/gr_welcome.au RTSP/1.0\r\nCSeq: %d\r\nTransport: rtp/avp;unicast;client_port=9000-9001;mode=\"PLAY\"\r\n\r\n",
                         cseqCounter++ );
                break;
            }
            case 'z':   //performance reocrd test
            {
                sprintf( buffer,"SETUP rtsp://www.vovida.com/twister%d.au RTSP/1.0\r\nCSeq: %d\r\nTransport: rtp/avp;unicast;client_port=%d-%d;mode=\"RECORD\"\r\n\r\n",
                         sessionId, cseqCounter++, cRand*2+9000, cRand*2+9000+1 );
                break;
            }
            case 'p':
            {
                sprintf( buffer,"PLAY rtsp://www.vovida.com/sounds/foo RTSP/1.0\r\nCSeq: %d\r\nSession: %d\r\n\r\n",
                         cseqCounter++, sessionId );
                break;
            }
            case '[':
            {
                sprintf( buffer,"PLAY rtsp://www.vovida.com/sounds/foo RTSP/1.0\r\nCSeq: %d\r\nSession: %d\r\nRange: ntp=2.0\r\n",
                         cseqCounter++, sessionId );
                break;
            }
            case 'r':
            {
                sprintf( buffer,"RECORD rtsp://www.vovida.com/sounds/foo RTSP/1.0\r\nCSeq: %d\r\nSession: %d\r\n\r\n",
                         cseqCounter++, sessionId );
                break;
            }
            case 'o':
            {
                sprintf( buffer,"PAUSE rtsp://www.vovida.com/sounds/LINEAR16.raw RTSP/1.0\r\nCSeq: %d\r\nSession: %d\r\n\r\n",
                         cseqCounter++, sessionId );
                break;
            }
            case 't':
            {
                sprintf( buffer,"TEARDOWN rtsp://www.vovida.com/sounds/LINEAR16.raw RTSP/1.0\r\nCSeq: %d\r\nSession: %d\r\n\r\n",
                         cseqCounter++, sessionId );
                break;
            }
            case '0':
            {
                sprintf( buffer,"TEARDOWN rtsp://www.vovida.com/sounds/LINEAR16.raw RTSP/1.0\r\nCSeq: %d\r\nSession: Global\r\n\r\n",
                         cseqCounter++ );
                break;
            }
            case '#':
            {
                printf("Generating RTP traffic to %s:%d\n", rmtHost.c_str(), sessionId);
                fflush(stdout);
                RtpSession stack( rmtHost.c_str(), sessionId, 0, 0, 0 );
                RtpPacket* packet = stack.createPacket();
                memset (packet->getPayloadLoc(), packet->getPayloadSize(), 0);
                packet->setPayloadUsage (packet->getPayloadSize());
                for( int i = 0; i < 1000; i++ )
                {
                    usleep(18*1000);
                    if( stack.transmit(packet) < 0 )
                        cerr<<"x";
                }
                int sendPkts = stack.getRtpTran()->getUdpStack()->getPacketsTransmitted();
                printf("Udp send pkts: %d\n", sendPkts);
                delete packet;
                continue;
            }
            case '1':
            {
                printf("Sleeping 1 sec\n");
                sleep( 1 );
                continue;
            }
            case '5':
            {
                printf("Sleeping 5 sec\n");
                sleep( 5 );
                continue;
            }
            case '!':
            {
                printf("Done\n");
                return 0;
            }
            default:
            {
                printf("Invalid choice\n");
                continue;
            }
        }
            
        if( !quit )
        {
            cerr<<"Sending rtsp message:\n"<<buffer;
            writeData( cSock.getConn(), buffer, strlen(buffer) );
            cerr <<"Waiting for reply\n";
            if (cSock.getConn().isReadReady(2))
            {
                int bytesRead = readData( cSock.getConn(), buffer, 1024 );
                int bytesParsed = 0;
                RtspMsgParser parser;
                // only try to get one msg from this buffer
                response = parser.preParse( buffer, bytesRead, bytesParsed, 
                                            savedMsg );
                if( response->getCompletionFlag() == RTSP_MSG_COMPLETE )
                {
                    savedMsg = 0;
                }
                else
                {
                    savedMsg = response;
                }
            }
        }
    }

    cout <<"Closing connection\n";
    cSock.close();

    cout <<"Done\n";
    return 0;
}


int
writeData( Connection conn, char* buffer, int len )
{
    cpLog( LOG_DEBUG_STACK, "Sending to tcp socket:\n%s", buffer );

    // write buffer to TCP connection
    try
    {
        conn.writeData( buffer, len );
    }
    catch( VNetworkException& e )
    {
        cerr <<e.getDescription()<<endl;
        return 0;
    }
    return 0;
}


int
readData( Connection conn, char* buffer, size_t len )
{
    int bytesRead;
    RtspTransceiver::readData( conn, (void*)buffer, len, bytesRead);
    if (bytesRead > 0)
    {
        Data dRead(buffer, bytesRead);
        // this is ready done in stack
        //cout <<"Server responded: \n";
        //cout <<"---------------------\n";
        //cout <<"bytesRead= " << bytesRead << endl;
        //cout <<dRead.getData()<<endl;
        //cout <<"---------------------\n";
        return bytesRead;
    }
    else
    {
        cout << "Server no response \n";
        return 0;
    }
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
