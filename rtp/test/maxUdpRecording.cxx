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

static const char* const maxRecording_cxx_Version =
    "$Id: maxUdpRecording.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "TimeTracker.hxx"
#include "support.hxx"
#include "UdpStack.hxx"

int main (int argc, char *argv[])
{
    //cpLogSetPriority( LOG_DEBUG_STACK );

    // allocate and initialize default configuration values
    // note, you may need to create two executable, each with the
    // others address info
    char rmtHost[256] = "vvs-tennis";
    int max_sessions = 100;
    int rmtPort = 7000;
    int lclPort = 5000;
    int numPkts = 10000;

    TimeTracker T(cerr);

    if (argc == 1)
    {
        cout << "Usage: ./maxRtpSessions host sessions rmt_start_port lcl_start_port num_packets" << endl;
        return 0;
    } 
    else if (argc == 6)
    {
        strcpy( rmtHost, argv[1] );
        max_sessions = atoi( argv[2] );
        rmtPort = atoi( argv[3] );
        lclPort = atoi( argv[4] );
        numPkts = atoi( argv[5] );

    }
    else
    {
        printf("Enter remote hostname: ");
        scanf ("%s", rmtHost);
        printf("Enter number of the max rtp sessions: ");
        scanf ("%d", &max_sessions);
        printf("Enter remote port number: ");
        scanf ("%d", &rmtPort);
        printf("Enter local port number: ");
        scanf ("%d", &lclPort);
        printf("Enter number of packets sending or receiving: ");
        scanf ("%d", &numPkts);

    }

    pid_t pid;

    int seqnum1 = -1;
    int seqnum2 = -1;

    for (int j=0; j < max_sessions; j++)
    {
        if ((pid = fork()) < 0)
        {
            cerr << "Fork failed." << endl;
            return -1;
        }
        else if (pid == 0)
        {
            cout << "Get pid is: " << getpid() <<endl;
            cout << "Creating udp session " << j << endl;


            //NetworkAddress* netAddressPtr = 0;
            //string rHostName = rmtHost;
            //NetworkAddress netAddress(rmtPort + 2*j);
            //netAddressPtr = &netAddress;
            //netAddressPtr->setHostName(rHostName);
            string rmtHostString = rmtHost;
            NetworkAddress dest( rmtHostString, rmtPort+2*j );

            UdpStack* stack = new UdpStack ( &dest, lclPort+2*j, lclPort+2*j, sendrecv );
            stack->connectPorts();
            UdpStack* stack1 = new UdpStack ( &dest, lclPort+2*j+1, lclPort+2*j+1, sendrecv );
            stack1->connectPorts();
            //UdpStack* stack1 = new UdpStack ( NULL, lclPort + 2*j+1 );
            //UdpStack* stack2 = new UdpStack ( netAddressPtr, rmtPort + 2*j );
            //UdpStack* stack3 = new UdpStack ( netAddressPtr, rmtPort + 2*j+1 );
                                  //rmtPort + 2*j, lclPort + 2*j );
                                  //rmtPort + 2*j + 1, lclPort + 2*j + 1, 
                                  //rtpPayloadPCMU, rtpPayloadPCMU, 0);

            char inPacketBuf[480];
        
            string filename = itos((unsigned int) j);
            filename.append(".data");
            ofstream file(filename.c_str(), ios::out);
            if (!file)
            {
                cerr << "Could not open file: " << filename << endl;
                return 3;
            }

            int i = 0;
            int cc;
            while( i < numPkts ) // assumming allowing 5 packets lost
            {
                // receive a packet
                cc = stack->receive( inPacketBuf, 480 );
                if( cc != 0 )
                {
                    seqnum2 = inPacketBuf[0];
                    if( seqnum1 == -1 )
                    {
                        T.startTime();
                        seqnum1 = seqnum2;
                    }
                    i++;
                    file.write( inPacketBuf, cc-1 );

                    if( inPacketBuf[10] == 127 )
                    {
                        break;
                    }
                }
        
/*
                // send and receive RTCP packet
                if (stack->checkIntervalRTCP())
                {
                    stack->transmitRTCP();
                }
                if (stack->receiveRTCP() == 1)
                {
                   break;
                }
*/
                usleep(1*1000);

            }
        
            //stack->transmitRTCPBYE();
            inPacketBuf[10] = 128;
            stack->transmit( inPacketBuf, 480 );        

            T.endTime();

            cout << "get packets: " << i << endl;
            cerr << " start seq num = " << seqnum1 << endl;
            cout << "   end seq num = " << seqnum2 << endl;
            cout << "End rtp session " << j << endl << endl;
            return 3;
        }
    }

    return 0;
}
