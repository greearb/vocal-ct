
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
/*-------------------------------< RCS >--------------------------------*/
static char RCS_ID[] = "$Id: tpjackd.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";
/*----------------------------< Defines >-------------------------------*/
/*----------------------------< Includes >------------------------------*/
/* stdlib */
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
//#include <varargs.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>

/* other */
#include <iostream>
#include "ixjuser.h"
#include "RtpSession.hxx"


/*---------------------------< Definitions >----------------------------*/
#define QLEN 5
#define BUFSIZE 256
/*--------------------------< Declarations >----------------------------*/
static int RTPjack(int fd);
static int passiveTCP(char *service, int qlen);
static int passivesock(char *service, char *protocol, int qlen);
static void zombie_kill(int n);

int IdentifyPeer(int nSock);
int GetNameFromAddr(char* szIP, char* szName, int nLenName);


/*------------------------< Global Variables >--------------------------*/
char szClientName[50];
char szClientAddr[50];
/*-------------------------< Local Variables >--------------------------*/
static char szDevice[32];
static u_short portbase = 0;
static int nPort = 7000;

int
daemon_init(void)
{
    pid_t pid;

    pid = fork();
    if (pid < 0) return -1;
    else if (pid != 0) exit(0);  /* parent exits */
    /* child continues and become session leader */
    setsid();
    return 0;
}

/*----------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
    struct sockaddr_in fsin;
    int msock, ssock;
    unsigned int alen;
    switch (argc)
    {
        case 2:
        // strncpy(szDevice,"/dev/ixj0",sizeof(szDevice));
        strncpy(szDevice, "/dev/phone0", sizeof(szDevice));
        nPort = atoi(argv[1]);
        break;
        //    case 3:
        //      strncpy(szDevice,argv[1],sizeof(szDevice));
        //      nPort=atoi(argv[2]);
        //      break;
        default:
        //      fprintf(stderr,"usage: tpjackd dev [port]\n");
        //      fprintf(stderr," - dev is probably /dev/ixj0\n");
        fprintf(stderr, "usage: tpjackd port\n");
        exit(1);
    }
    msock = passiveTCP(argv[1], QLEN);

    /* make this program a daemon */
    daemon_init();
    signal(SIGCHLD, zombie_kill);

    while (1)
    {
        alen = sizeof(fsin);
        ssock = accept(msock, (struct sockaddr *) & fsin, &alen);

        if (ssock < 0)
        {
            if (errno == EINTR) continue;
            printf("accept: %s\n", sys_errlist[errno]);
        }
        switch (fork())
        {
            case 0:
            close(msock);
            exit(RTPjack(ssock));
            default:
            close(ssock);
            break;
            case - 1:
            printf("fork: %s\n", sys_errlist[errno]);
        }
    }
}
/*----------------------------------------------------------------------*/
static int
RTPjack(int fd)
{
    int cc, ixj, read_fd, send_fd, readc_fd, sendc_fd, n, hook, m_hook, retval, nMax;
    fd_set rfds;
    struct timeval tv;

    /* the fd passed in is the socket descriptor for the signalling socket
       first, we use it to identify the client 
    */
    n = IdentifyPeer(fd);
    if (n < 0) printf("failure on signalling socket\n");
    printf("connection from %s\n", szClientName);
    fflush(stdout);

    ixj = open(szDevice, O_RDWR);
    if (ixj < 0) printf("failure opening %s\n", szDevice);

    if (ioctl(ixj, IXJCTL_RING))
    {
        /* connect the data ports */
        RtpSession dataStack(szClientAddr, nPort + 1, nPort + 1, nPort + 2, nPort + 2);
        RtpPacket* outPacket = dataStack.createPacket(BUFSIZE);
        RtpPacket* inPacket = NULL;

        read_fd = (dataStack.getRtpRecv())->getSocketFD();
        send_fd = (dataStack.getRtpTran())->getSocketFD();
        readc_fd = (dataStack.getRtcpRecv())->getSocketFD();
        sendc_fd = (dataStack.getRtcpTran())->getSocketFD();

        if ((read_fd < 0) || (send_fd < 0) || (readc_fd < 0) || (sendc_fd < 0))
            printf("failure on RTP socket\n");

        ioctl(ixj, IXJCTL_PLAY_CODEC, ULAW);
        ioctl(ixj, IXJCTL_REC_CODEC, ULAW);
        ioctl(ixj, IXJCTL_REC_START);
        ioctl(ixj, IXJCTL_PLAY_START);
        ioctl(ixj, IXJCTL_AEC_START);

        while (hook = ioctl(ixj, IXJCTL_HOOKSTATE))
        {
            FD_ZERO(&rfds);
            FD_SET(read_fd, &rfds);
            FD_SET(readc_fd, &rfds);
            FD_SET(ixj, &rfds);
            tv.tv_sec = 0;
            tv.tv_usec = 300;
            retval = select(nMax, &rfds, NULL, NULL, &tv);
            if (FD_ISSET(ixj, &rfds))                    // read data from phone
            {
                cc = read(ixj, outPacket->getPayloadLoc(), 240);
                outPacket->setRtpTime(dataStack.getPrevRtpTime() + 240);
                if (cc > 0)
                {
                    assert (cc == 240);
                    outPacket->setPayloadUsage(cc);
                    outPacket->setRtpTime(dataStack.getPrevRtpTime() + 240);
                    dataStack.transmit(outPacket);
                }
            }
            if (1 || FD_ISSET(read_fd, &rfds))                // read data from network
            {
                inPacket = dataStack.receive();
                if (inPacket)
                {
                    assert (inPacket->getPayloadUsage() == 240);
                    write(ixj, inPacket->getPayloadLoc(), inPacket->getPayloadUsage());
                }
                else
                {
                    /* if(errno!=EAGAIN)
                       {
                         ioctl(ixj, IXJCTL_REC_STOP);
                         ioctl(ixj, IXJCTL_PLAY_STOP);
                         ioctl(ixj, IXJCTL_AEC_STOP);
                         printf("echo read: %s\n",sys_errlist[errno]);
                       } */
                }
            }
            dataStack.processRTCP();
        } /* end while() */
    }

    close(read_fd);
    close(send_fd);
    close(readc_fd);
    close(sendc_fd);
    close(fd);
    close(ixj);
    return 0;
}
/*----------------------------------------------------------------------*/
static int
passiveTCP(char *service, int qlen)
{
    return passivesock(service, "tcp", qlen);
}
/*----------------------------------------------------------------------*/
static int
passivesock(char *service, char *protocol, int qlen)
{
    struct servent *pse;
    struct protoent *ppe;
    struct sockaddr_in sin;
    int s, type;

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    if (pse = getservbyname(service, protocol))
        sin.sin_port = htons(ntohs((u_short)pse->s_port) + portbase);
    else if ((sin.sin_port = htons((u_short)atoi(service))) == 0)
        printf("can't get \"%s\" service entry\n", service);

    if ((ppe = getprotobyname(protocol)) == 0)
        printf("can't get \"%s\" protocol entry\n", protocol);

    if (strcmp(protocol, "udp") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    s = socket(PF_INET, type, ppe->p_proto);

    if (s < 0)
        printf("can't create socket: %s\n", sys_errlist[errno]);

    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        printf("can't bind to %s port: %s\n", service, sys_errlist[errno]);

    if (type == SOCK_STREAM && listen(s, qlen) < 0)
        printf("can't listen on %s port: %s\n", service, sys_errlist[errno]);

    return s;
}
/*----------------------------------------------------------------------*/
int
IdentifyPeer(int nSock)
{
    struct sockaddr_in sPeer;
    int n;
    unsigned int	nSize;
    char* p;

    /* get the name and address of the client */
    nSize = sizeof(sPeer);
    n = getpeername(nSock, (struct sockaddr*) & sPeer, &nSize);
    if (n == -1)
    {
        printf("Unable to get peer name info\n");
        return -1;
    }
    p = inet_ntoa(sPeer.sin_addr);
    strncpy(szClientAddr, p, sizeof(szClientAddr));
    GetNameFromAddr(szClientAddr, szClientName, sizeof(szClientName));
    return 1;
}
/*----------------------------------------------------------------------*/
int
GetNameFromAddr(char* szIP, char* szName, int nLenName)
{
    struct sockaddr_in sPeer;
    int n;
    struct hostent* pHe;

    /* get the name and address of the IP address passed in */
    n = inet_aton(szIP, &sPeer.sin_addr);
    if ((pHe = gethostbyaddr((char*) & sPeer.sin_addr, sizeof(sPeer.sin_addr),
                             AF_INET)) == NULL)
    {
        printf("Unable to get peer address info\n");
        return -1;
    }
    memset(szName, 0x00, nLenName);
    strncpy(szName, pHe->h_name, nLenName - 1);
    return 1;
}
/*----------------------------------------------------------------------*/
static void
zombie_kill(int n)
{
    union wait status;
    while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}
/*-------------------------------< End >--------------------------------*/
