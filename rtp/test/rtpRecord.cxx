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

static const char* const rtpRecord_cxx_Version =
    "$Id: rtpRecord.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "global.h"
#include <unistd.h>
#include <fstream>

#include "RtpSession.hxx"
#include "cpLog.h"
#include "NtpTime.hxx"

#ifdef __vxworks
#include "VThread.hxx"
#endif


int main (int argc, char *argv[])
{
    cpLogSetPriority (LOG_DEBUG_HB);

    if (argc < 2)
    {
        cerr << "Usage: rtpRecord file [host]\n";
        exit (1);
    }
    ofstream file(argv[1], ios::app);

    if (!file)
    {
        cerr << "Could not open file '" << argv[1] << "'\n";
        return -1;
    }

    char* host = NULL;
    if (argc == 3)
        host = argv[2];
    else
        host = "localhost";

    // receiving on localhost on port 5000/5001
    // use seperate RTCP ports for localhost usage
    RtpSession stack (host, 9002, 9000, 9003, 9001);
    stack.setApiFormat (rtpPayloadPCMU, 160);
    stack.setNetworkFormat (rtpPayloadPCMU, 160);
    RtpPacket* p = NULL;

    // stop loop after 400 packet attempts
    NtpTime nowTime, pastTime;
    for (int i = 0; i < 999; i++)
    {
        p = stack.receive();
        if (p)
        {
            // got a packet - write data to file
            file.write(p->getPayloadLoc(), p->getPayloadUsage());
            cerr << "*";
            // simulate blocking write
#ifdef __vxworks
            sleep(10);
#else
	    usleep (10*1000);
#endif
        }
        else
        {
            // no packet received
            cerr << "x";
#ifdef __vxworks
            sleep(5);
#else
            usleep (5*1000);
#endif

        }

        // send and receive RTCP packet
        stack.processRTCP();
    }

    return 0;
}
