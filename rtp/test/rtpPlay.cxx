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

static const char* const rtpPlay_cxx_Version =
    "$Id: rtpPlay.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


// --- Authors ---------------------------------------------------- //
// Cullen Jennings, April 1999
// Kim Le, June 1999
// ---------------------------------------------------------------- //

#include "global.h"
#include "RtpSession.hxx"
#include <fstream>
#include <iostream>

#ifdef __vxworks
#include "VThread.hxx"
#endif
#include "cpLog.h"

int main (int argc, char *argv[])
{
    cpLogSetPriority (LOG_DEBUG_HB);

    if (argc < 2)
    {
        cerr << "Usage: rtpPlay file [host]\n";
        exit (1);
    }

    ifstream file(argv[1]);
    if (!file)
    {
        cerr << "Could not open file\n";
        return -1;
    }

    char* host = NULL;
    if (argc >= 3)
        host = argv[2];
    else
        host = "localhost";

    int serverPort = 9000;
    if (argc >= 4)
        serverPort = atoi(argv[3]);

    // sending to localhost on port 5000/5001
    // use seperate RTCP ports for localhost usage
    RtpSession stack (host, serverPort, 9002, serverPort+1, 9003);
    //ApiFormat is application codec and timing rate
    // 160 = 20ms for PCMU
    // 240 = 30ms for PCMU
    stack.setApiFormat (rtpPayloadPCMU, 160);
    //NetworkFormat is network codec and timing rate
    // 160 = 20ms for PCMU
    stack.setNetworkFormat (rtpPayloadPCMU, 160);
    RtpPacket* packet = NULL;

    // set SDES information - optional
    RtcpTransmitter* rtcpTran = stack.getRtcpTran();
    rtcpTran->setSdesEmail("development@vovida.com");


    // send out data until end of file
    int result;
    int prevRtpTime = 4294960000;
    int i = 0;
    while (!file.eof())
    {
        packet = stack.createPacket();

        // load data from file into packet
        file.read (packet->getPayloadLoc(), packet->getPayloadSize());
        packet->setPayloadUsage (file.gcount());

        // create silence gap every 10 packets
        if( i++ % 10 == 0 )
        {
            cerr <<"s";
            prevRtpTime += 160;

            // load new data from file into packet
            file.read (packet->getPayloadLoc(), packet->getPayloadSize());
            packet->setPayloadUsage (file.gcount());
        }

        packet->setRtpTime( prevRtpTime );
        prevRtpTime += 160;

        // simulating blocking read call
        usleep (10*1000);
        result = stack.transmit(packet);
        if (result < 0)
            cerr << "x";
        else
            cerr << ".";
        delete packet; packet = NULL;

        // send and receive RTCP packet
        //stack.processRTCP();
    }
    file.close();

    // leaving session
#ifdef __vxworks
    sleep(50);
#else
    usleep (50*1000);
#endif
    stack.transmitRTCPBYE();

    RtpTransmitter* tran = stack.getRtpTran();
    cerr << endl;
    cerr << "Statistics for SRC " << tran->getSSRC() << endl;
    cerr << "Packets sent: " << tran->getPacketSent() << "     ";
    cerr << "Payload sent: " << tran->getPayloadSent() << "     ";
    cerr << endl;

    return 0;
}
