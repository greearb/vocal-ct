
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
static const char* const test_udp_cxx_Version =
    "$Id: test_udp.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "NetworkAddress.h"
#include "UdpStack.hxx"
#include "cpLog.h"
#include "VTime.hxx"
#include <unistd.h>
#include <stdio.h>

#ifdef __vxworks
#include "VThread.hxx"
#include <resolvLib.h>
#endif


// yet to be tested: multicase

int main(int argc, char *argv[])
{
    cpLogSetPriority(LOG_DEBUG);

    char outMsg[100] = "abcdedfg";
    char inMsg[100] = "";

    // allocate and initialize/hardcode configuration parameters
    // note, you may need to create two executable, each with the
    // others address info
    char rmtHost[256] = "192.168.5.100";
    int rmtPort = 7000;
    int lclPort = 8000;

#ifndef __vxworks
    // if not VxWorks, set config parameters at runtime;
    printf("Enter remote hostname: ");
    scanf ("%s", rmtHost);
    printf("Enter remote port number: ");
    scanf ("%d", &rmtPort);
    printf("Enter local port number: ");
    scanf ("%d", &lclPort);
#else
    resolvInit("192.168.5.254", "private.vovida.com", 0);
#endif // __vxworks

    NetworkAddress remoteHost(rmtHost, rmtPort);
    UdpStack stack(&remoteHost, lclPort, lclPort, sendrecv);

    stack.connectPorts();

    int len;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    for (int i = 0; i < 1000; i++)
    {
        // every time need to reset FD_SET
        fd_set netFD;
        FD_ZERO (&netFD);
        FD_SET (stack.getSocketFD(), &netFD);

        int selret = select (stack.getSocketFD() + 1,
                             &netFD, NULL, NULL, &timeout);

        if (selret > 0)
        {
            len = stack.receive(inMsg, 100);
            if (len)
            {
                printf("r");
                inMsg[len] = '\n';
            }
            else
            {
                printf( " rcv empty pkt ");
            }
        }
        else
        {
            printf(" no  pkt ");
        }

        usleep (50000);  // 50 msec
        stack.transmit(outMsg, 100);
        printf("t");
        fflush(stdout);
    }
}
