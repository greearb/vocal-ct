
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
static const char* const tpTrunk_org_cxx_Version =
    "$Id: tpTrunk_org.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";
// --- Authors ---------------------------------------------------- //
// Kim Le, July 1999
// ---------------------------------------------------------------- //

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>

#include "cpLog.h"

#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "ixjuser.h"

#define ULAW_PAYLOAD 240


int main (int argc, char *argv[])
{
    RtpPayloadType payloadType = rtpPayloadPCMU;
    char* buffer = new char [512];
    char* outbuffer = new char [512];
    int cc;

    if (argc != 4)
    {
        cerr << "Usage: tpTrunk host rtp_tran_port rtp_recv_port\n";
        return 1;
    }

    // open quicknet card
    // char* szDevice = "/dev/ixj0";
    char* szDevice = "/dev/phone0";
    printf("Opening Phone Jack (%s)\n", szDevice);
    int ixj = open(szDevice, O_RDWR);
    if (ixj < 0)
    {
        perror("Error opening voice card");
        return -1;
    }


    // open RTP stack
    RtpSession stack (argv[1], atoi(argv[2]), atoi(argv[3]),
                      atoi(argv[2]) + 1, atoi(argv[3]) + 1,
                      payloadType);
    RtpPacket* rPacket = NULL;
    RtpPacket* sPacket = NULL;
    RtpPacket* tPacket = NULL;
    RtpPacket* outPacket = stack.createPacket(160);
    outPacket->setPayloadUsage (160);

    /*
        // wait for off hook
        int ring = 0;
        int hook = ioctl(ixj, IXJCTL_HOOKSTATE);
        while(hook != 1)
        {
            hook = ioctl(ixj, IXJCTL_HOOKSTATE);
            rPacket = stack.receive ();
            if (rPacket)
            {
                if (!ring)
                {
                    cerr <<"Ring phone\n";
                    ioctl(ixj, IXJCTL_RING);
                }
                ring = 1;
                printf("Waiting for hookswitch\n");
            }
            delete rPacket;
        }
    */

    // open audio path
    ioctl(ixj, IXJCTL_PLAY_CODEC, ULAW);
    ioctl(ixj, IXJCTL_REC_CODEC, ULAW);
    //ioctl(ixj,IXJCTL_FRAME,20);
    ioctl(ixj, IXJCTL_REC_START);
    ioctl(ixj, IXJCTL_PLAY_START);
    ioctl(ixj, IXJCTL_AEC_START);


    // continue audio path
    while (ioctl(ixj, IXJCTL_HOOKSTATE))
    {

        // receive 20ms packet r
        while (1)
        {
            rPacket = stack.receive ();
            if (!rPacket) continue;
            if (rPacket->getPayloadUsage() != 160)
            {
                delete rPacket;
                rPacket = NULL;
                cerr << "R";
                continue;
            }
            break;
        }


        // read from card first 30ms part
        cc = read(ixj, buffer, 240);
        if (cc != 240) cerr << "M";
        //        else  cerr <<"m";

        // send 20ms packet x
        memcpy (outPacket->getPayloadLoc(), buffer, 160);
        outPacket->setRtpTime(stack.getPrevRtpTime() + 160);
        stack.transmit (outPacket);
        //        cerr <<"x";

        // receive 20ms packet s
        while (1)
        {
            sPacket = stack.receive ();
            if (!sPacket) continue;
            if (sPacket->getPayloadUsage() != 160)
            {
                delete sPacket;
                sPacket = NULL;
                cerr << "S";
                continue;
            }
            break;
        }

        // write to card first 30ms part
        memcpy (outbuffer, rPacket->getPayloadLoc(), 160);
        memcpy (outbuffer + 160, sPacket->getPayloadLoc(), 160);
        if (rPacket)
        {
            delete rPacket;
            rPacket = NULL;
        }
        if (sPacket)
        {
            delete sPacket;
            sPacket = NULL;
        }
        write (ixj, outbuffer, 240);

        // read from card second 30ms part
        cc = read(ixj, buffer + 240, 240);
        if (cc != 240) cerr << "N";
        //        else  cerr <<"n";

        // send 20ms packet y
        memcpy (outPacket->getPayloadLoc(), buffer + 160, 160);
        outPacket->setRtpTime(stack.getPrevRtpTime() + 160);
        stack.transmit (outPacket);
        //        cerr <<"y";

        // receive 20ms packet t
        while (1)
        {
            tPacket = stack.receive ();
            if (!tPacket) continue;
            if (tPacket->getPayloadUsage() != 160)
            {
                delete tPacket;
                tPacket = NULL;
                cerr << "T";
                continue;
            }
            break;
        }

        // write to card second 30ms part
        if (160 != tPacket->getPayloadUsage()) cerr << "Y";
        memcpy (outbuffer + 160 + 160, tPacket->getPayloadLoc(), 160);
        if (tPacket)
        {
            delete tPacket;
            tPacket = NULL;
        }
        write (ixj, outbuffer + 240, 240);

        // send 20ms packet z
        memcpy (outPacket->getPayloadLoc(), buffer + 160 + 160, 160);
        outPacket->setRtpTime(stack.getPrevRtpTime() + 160);
        stack.transmit (outPacket);
        //        cerr <<"z";


        // send and receive RTCP packet
        // seperate to debug when packets are sent and recv
        if (stack.checkIntervalRTCP())
        {
            stack.transmitRTCP();
            cerr << "A";
        }
        if (!stack.receiveRTCP())
            cerr << "B";
    }
    stack.transmitRTCPBYE();

    ioctl(ixj, IXJCTL_REC_STOP);
    ioctl(ixj, IXJCTL_PLAY_STOP);
    ioctl(ixj, IXJCTL_AEC_STOP);
    close(ixj);

    return 0;
}
