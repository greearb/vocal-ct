
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





static const char* const udpInjectRcv_cxx_Version =
    "$Id: udpInjectRcv.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include <unistd.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream>
#include <string.h>
#include <string>
#include <netinet/in.h>
//#include <sys/socket.h>
#include "vsock.hxx"
#include "NetworkAddress.h"
#ifdef __vxworks
#include "VThread.hxx"
#endif
#include "VCondition.h"

int main (int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: udpInjectRcv file [host]\n";
        return ( -1);
    }

    ifstream file(argv[1]);
    if (!file)
    {
        cerr << "Could not open file\n";
        return -1;
    }

    char* host = NULL;
    if (argc == 3)
        host = argv[2];
    else
        host = "localhost";

    NetworkAddress netAddress;
    struct sockaddr_in xDest;


    char line[256];
    char pkt[4096];


    if (!file.eof())
    {
        memset (line, 0, 256);
        file.getline(line, 256);
        cout << line << endl;
    }

    while (!file.eof())
    {

        int ret;

        ret = strcmp(line, "UdpRcv");
        if (ret != 0)
        {
            cout << "Input file is wrong." << endl;
            return -1;
        }

        memset (line, 0, 256);
        file.getline(line, 256);
        cout << line << endl;

        char tag[256];
        char str_port[256];
        ret = sscanf(line, "%s%s", tag, str_port) ;

        //        cout << ret << endl;
        //        cout << tag << endl;
        //        cout << str_port << endl;

        int locPort = atoi(str_port);
        if (locPort <= 0)
        {
            cout << "Local port invalid." << endl;
            return -1;
        }

        if ( host)
        {
            string rHostName = host;
            netAddress.setPort(locPort);
            netAddress.setHostName(rHostName);
        }

        xDest.sin_family = AF_INET;
        xDest.sin_port = htons(netAddress.getPort());
        xDest.sin_addr.s_addr = netAddress.getIp4Address();


        int socketFd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socketFd < 0)
        {
            cout << "Error create socket." << endl;
            return -1;
        }

        if (!file.eof())
        {
            memset (line, 0, 256);
            file.getline(line, 256);
            cout << line << endl;
        }
        while (!file.eof())
        {
            int seqNum = 0;
            int len = 0;
            char str_seqNum[256];
            char str_len[256];

            ret = sscanf(line, "%s%s", str_seqNum, str_len) ;

            seqNum = atoi(str_seqNum);
            len = atoi(str_len);

            memset (pkt, 0, 4096);
            file.read(pkt, len);

            int count = sendto( socketFd,
                                (char *) pkt,
                                len,
                                0 /* flags */ ,
                                (struct sockaddr*) & xDest,
                                sizeof(sockaddr_in));
            if (count < 0)
            {
                cout << "error happen when sendto(). " << seqNum << endl;
                return -1;
            }
            else
            {
                cout << "send pkt #" << seqNum << endl;
            }

            vusleep (200);

            memset (line, 0, 256);
            file.getline(line, 256);
            cout << line << endl;

            memset (line, 0, 256);
            file.getline(line, 256);
            cout << line << endl;

            if (!file.eof())
            {
                memset (line, 0, 256);
                file.getline(line, 256);
                cout << line << endl;
            }

        }
        if (!file.eof())
        {
            memset (line, 0, 256);
            file.getline(line, 256);
            cout << line << endl;
        }

    }
    file.close();

    return 0;
}
