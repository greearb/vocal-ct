
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

static const char* const snmptest_cxx_Version =
    "$Id: snmptest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "NetworkAddress.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "NetworkAddress.h"
#include "AgentApi.hxx"
#include "UdpStack.hxx"
#include <stdlib.h>
#include <iostream.h>
#include "SipAgent.hxx"
#include "Data.hxx"


int main(int argc , char **argv)
{
    int retVal;
    char *mibvariable;
    char *ipaddress;
    char *portno;
    if (argc != 3)
    {
        cout << "Usage:mibvariable ip port\n";
        exit( -1);
    }

    ipaddress = (char *) argv[1];
    mibvariable = (char *)argv[2];

    //NetworkAddress rxNetAddr("192.168.5.140",SIPSTACKPORT);
    NetworkAddress rxNetAddr(ipaddress, SIPSTACKPORT);
    unsigned int iv = 23;
    Data ccv = (Data)iv;
    ipcMessage reqMsg, respMsg;
    int ind = 0;

    UdpStack udpStack((NetworkAddress *)&rxNetAddr, 10001, 10001, sendrecv, false);
    cout << " I have sent successfully" << endl;

    reqMsg.action = Get;
    reqMsg.mibVariable = (AgentApiMibVarT) * mibvariable;
    reqMsg.transactionNumber = 2;

    udpStack.transmitTo((char *)&reqMsg, sizeof(ipcMessage), &rxNetAddr);
    int a = udpStack.receiveFrom((char *) & respMsg, sizeof(ipcMessage), &rxNetAddr);
    cout << "Successful" << " " << *(uint *)respMsg.parm2 << endl;
    return 0;

}
