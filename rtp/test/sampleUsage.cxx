
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

static const char* const sampleUsage_cxx_Version =
    "$Id: sampleUsage.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";
// --- Authors ---------------------------------------------------- //
// Kim Le, July 1999
// ---------------------------------------------------------------- //

#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include "cpLog.h"
#include "rtpTypes.h"
#include "RtpSession.hxx"

#ifdef __vxworks
#include <resolvLib.h>
#include "VThread.hxx"
#endif

int main (int argc, char *argv[])
{
#ifdef __vxworks
    resolvInit("192.168.5.254", "private.vovida.com", 0);
#endif

    // allocate and initialize default configuration values
    // note, you may need to create two executable, each with the
    // others address info
    char rmtHost[256] = "192.168.5.11";
    int rmtPort = 6000;
    int lclPort = 5000;
    int apSize = 100;
    int npSize = 100;

#ifndef __vxworks
    // override default values if not vxworks
    if (argc == 6)
    {
        // Usage:"sampleUsage host rtpTxPort rtpRxPort
        //                    apiPktSize networkPktSize
        strcpy( rmtHost, argv[1] );
        rmtPort = atoi( argv[2] );
        lclPort = atoi( argv[3] );
        apSize = atoi( argv[4] );
        npSize = atoi( argv[5] );
    }
    else
    {
        printf("Enter remote hostname: ");
        scanf ("%s", rmtHost);
        printf("Enter remote port number: ");
        scanf ("%d", &rmtPort);
        printf("Enter local port number: ");
        scanf ("%d", &lclPort);
        printf("API packet size: ");
        scanf ("%d", &apSize);
        printf("Network packet size: ");
        scanf ("%d", &npSize);
    }
#endif

    RtpSession* stack = new RtpSession ( rmtHost, rmtPort, lclPort,
                                         rmtPort + 1, lclPort + 1, rtpPayloadPCMU, rtpPayloadPCMU, 5);

    // setup logfile
    cpLogSetLabel ("sampleUsage");
    //    cpLogSetPriority (LOG_DEBUG);

    RtpPacket* inPacket = NULL;

    RtpPacket* outPacket = stack->createPacket();

    // only 5 bytes of data, but still send a big packet
    strncpy (outPacket->getPayloadLoc(), "data", 5);

    stack->setApiFormat (rtpPayloadPCMU, apSize);
    stack->setNetworkFormat (rtpPayloadPCMU, npSize);

    for (int i = 0; i < 80; i++)
    {

        // receive a packet
        inPacket = stack->receive ();
        if (inPacket)
        {
            //            if (i<2)
            //                inPacket->printPacket();
            printf( "r" );
            delete inPacket;
            inPacket = NULL;
        }
        else
        {
            printf( "n");
        }


        // sleep between rcv and trans
        usleep (100*1000);  // 100 msec

        // send a packet
        outPacket->setRtpTime(stack->getPrevRtpTime() + npSize);
        //        if (i<2)
        //           outPacket->printPacket();
        outPacket->setPayloadUsage (npSize);

        stack->transmit (outPacket);

        printf( "t" );


        // send and receive RTCP packet
        if (stack->checkIntervalRTCP())
        {
            stack->transmitRTCP();
            printf( "T" );
        }
        if (stack->receiveRTCP() == 0 )
        {
            printf( "R" );
        }


        fflush(stdout);
    }

    stack->transmitRTCPBYE();


    if (outPacket)
    {
        delete outPacket;
        outPacket = NULL;
    }

    if (inPacket)
    {
        delete inPacket;
        inPacket = NULL;
    }

    delete stack;
    stack = NULL;

    return 0;
}
