
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
static const char* const test_sendrecv_send_cxx_Version =
    "$Id: test_sendrecv_send.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include <sys/time.h>
#include <iostream.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#if defined(__svr4__)
typedef int socklen_t;
#endif

int main ()
{
    int socketFD;
    struct sockaddr_in localAddr;
    int res1;

    socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset((char*) &(localAddr), 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(9020);

    res1 = bind( socketFD, (struct sockaddr*) & (localAddr), sizeof(localAddr));
    cerr << "res1= " << res1 << endl;

    /*
        fd_set netFD;
        FD_ZERO (&netFD);
        FD_SET (socketFD, &netFD);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 300;
     
        int selret = 0;
     
        while (selret <= 0 )
        {
             selret = select (socketFD + 1,
                             &netFD, NULL, NULL, &timeout);
        }
     
    */
    char buf[256];
    int bufSize = 256;
    memset (buf, 0, bufSize);

    struct sockaddr_in xSrc;
    unsigned int lenSrc = sizeof(xSrc);

    int len;
    len = recvfrom( socketFD,
                    (char *)buf,
                    bufSize,
                    0 /*flags */,
                    (struct sockaddr*) & xSrc,
                    (socklen_t *) & lenSrc);

    if (len <= 0)
    {
        cerr << "receive nothing" << endl ;
    }
    else
    {
        sendto( socketFD, buf, len, 0,
                (struct sockaddr*)&xSrc, sizeof(sockaddr_in));
    }


    return 0;
}
