
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


// This is a test to see how many rtp session can be established 
// between two linux boxes. It is based on sampleUsage.cxx

static const char* const maxRtpSessions_cxx_Version =
    "$Id: maxRtpSessions.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include "cpLog.h"
#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "TimeTracker.hxx"


int main (int argc, char *argv[])
{

    // allocate and initialize default configuration values
    // note, you may need to create two executable, each with the
    // others address info
    char rmtHost[256] = "vvs-softball";
    int max_sessions = 100;
    int rmtPort = 7000;
    int lclPort = 5000;
    int apSize = 160;
    int npSize = 160;

    TimeTracker T(cerr);

    if (argc == 1)
    {
        cout << "Usage: ./maxRtpSessions host sessions rmt_start_port lcl_start_port" << endl;
        return 0;
    } 
    else if (argc == 5)
    {
        strcpy( rmtHost, argv[1] );
        max_sessions = atoi( argv[2] );
        rmtPort = atoi( argv[3] );
        lclPort = atoi( argv[4] );

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

    }

    pid_t pid;


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
            cout << "Creating rtp session " << j << endl;

            T.startTime();

            RtpSession* stack = new RtpSession ( rmtHost, 
                                     rmtPort + 2*j, lclPort + 2*j,
                                     rmtPort + 2*j + 1, lclPort + 2*j + 1, 
                                     rtpPayloadPCMU, rtpPayloadPCMU, 5);

            // setup logfile
//            cpLogSetLabel ("maxRtpSessions");
            //    cpLogSetPriority (LOG_DEBUG);
        
            RtpPacket* inPacket = NULL;
        
            char outBufferPkt[480];
        
            // only 5 bytes of data, but still send a big packet
            strncpy (outBufferPkt, "data", 5);
        
            stack->setApiFormat (rtpPayloadPCMU, apSize);
            stack->setNetworkFormat (rtpPayloadPCMU, npSize);
        
            for (int i = 0; i < 10000; i++)
            {

                // receive a packet
                inPacket = stack->receive ();
                if (inPacket)
                {
                    //            if (i<2)
                    //                inPacket->printPacket();
//                    printf( "r" );
                    delete inPacket;
                    inPacket = NULL;
                }
                else
                {
//                    printf( "n");
                }
        
        
                // sleep between rcv and trans
                usleep (20*1000);  // 20 msec
        
                // send a packet
                //        if (i<2)
                //           outPacket->printPacket();
                stack->transmitRaw (outBufferPkt, 160);
        
//                printf( "t" );
        
        

                // send and receive RTCP packet
                if (stack->checkIntervalRTCP())
                {
                    stack->transmitRTCP();
//                    printf( "T" );
                }
                if (stack->receiveRTCP() == 0 )
                {
//                    printf( "R" );
                }

        
//                fflush(stdout);
            }
        
            stack->transmitRTCPBYE();
        
            if (inPacket)
            {
                delete inPacket;
                inPacket = NULL;
            }
        
            delete stack;
            stack = NULL;


            T.endTime();
            cout << "End rtp session " << j << endl;
            return 3;
//            break;
        }
/*
        else if (pid != 0)
        {
            cerr << "It is still parent process." << endl;
        }
*/
    }

    return 0;
}
