
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
static const char* const tran_cxx_Version =
    "$Id: tran.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";
// --- Authors ---------------------------------------------------- //
// Kim Le, Aug 1999
// ---------------------------------------------------------------- //
/*
    Sends 4000 byte packets every 500 ms
*/


#include <unistd.h>
#include <fstream>

#include "cpLog.h"

#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "Rtp.hxx"
#include "Rtcp.hxx"


int main ()
{
    // setup logfile
    cpLogSetPriority (LOG_DEBUG);

    // sending to localhost on port 5000/5001
    RtpSession stack ("localhost", 5000, 0, 5001, 5003);
    RtpPacket* packet = NULL;

    // set SDES information
    RtcpTransmitter* rtcpTran = stack.getRtcpTran();
    rtcpTran->setSdesEmail("development@vovida.com");

    // send out packets
    char* data = "dataxxxx";
    for (int i = 0; i < 400; i++)
    {
        packet = stack.createPacket(4000);

        // load data into packet
        strncpy(packet->getPayloadLoc(), data, strlen(data));
        packet->setPayloadUsage(4000);
        packet->setRtpTime(stack.getPrevRtpTime() + 4000);   // every 500 ms

        // send packet
        usleep(500*1000);
        if (stack.transmit(packet))
            cerr << "x";
        else
            cerr << ".";
        delete packet; packet = NULL;

        // send and receive RTCP packet
        stack.processRTCP();
    }

    // leaving session
    stack.transmitRTCPBYE();

    return 0;
}
