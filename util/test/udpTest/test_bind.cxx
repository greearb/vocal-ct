
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
static const char* const test_bind_cxx_Version =
    "$Id: test_bind.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include <iostream.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "errno.h"
#include <strstream>

int main ()
{
    int socketFD;
    struct sockaddr_in localAddr;
    struct sockaddr_in remoteAddr;
    struct sockaddr dummyAddr;
    int res1, res2, res3;
    char laddr[4];
    char laddr2[4];

    socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset((char*) &(localAddr), 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    //    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //    struct hostent* host = gethostbyname ("mendel");
    laddr[0] = 192;
    laddr[1] = 168;
    laddr[2] = 5;
    laddr[3] = 28;
    memcpy((char*)&(localAddr).sin_addr.s_addr,
           laddr,  // take the first entry
           4);


    localAddr.sin_port = htons(5000);

    res1 = bind( socketFD, (struct sockaddr*) & (localAddr), sizeof(localAddr));

    if (res1 != 0)
    {
        int err = errno;
        if ( err == EADDRINUSE )
        {
            cerr << "address in use" << endl;
        }

        strstream errMsg;
        errMsg << "UdpStack<"
        << ">::UdpStack error during socket bind: ";
        errMsg << strerror(err);
        errMsg << char(0);

        clog << errMsg.str() << endl;
        cerr << "bind error" << endl;
        //            throw UdpStackException(errMsg.str());
        errMsg.freeze(false);
        //            assert(0);

    }

    memset((char*) &(remoteAddr), 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    struct hostent* host = gethostbyname ("ford");

    laddr2[0] = 192;
    laddr2[1] = 168;
    laddr2[2] = 66;
    laddr2[3] = 3;
    memcpy((char*)&(remoteAddr).sin_addr.s_addr,
           laddr2,  // take the first entry
           4);
    /*
        memcpy((char*)&(remoteAddr).sin_addr.s_addr,
               host->h_addr_list[0], // take the first entry 
               host->h_length);
    */
    remoteAddr.sin_port = htons(7);

    res2 = sendto( socketFD, "hello!", 7, 0,
                   (struct sockaddr*) & remoteAddr, sizeof(sockaddr_in));

    cout << "res2=" << res2 << endl;
}
