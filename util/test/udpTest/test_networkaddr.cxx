
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
static const char* const test_networkaddr_cxx_Version =
    "$Id: test_networkaddr.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "global.h"
#include "NetworkAddress.h"
#include "cpLog.h"

int main(void)
{
    //    cpLogSetPriority(LOG_DEBUG);

    NetworkAddress addr1("192.168.5.11", 8080);
    NetworkAddress addr2("192.168.22.15", 8989);
    struct sockaddr socka;
    struct sockaddr socka2;
    addr1.getSockAddr(socka);
    addr2.getSockAddr(socka2);
    cout << socka.sa_family << endl;
    for (int i = 0; i < 14; i++)
        cout << (int) socka.sa_data[i] << " " ;
    cout << endl;
    cout << socka2.sa_family << endl;
    for (int j = 0; j < 14; j++)
        cout << (int) socka2.sa_data[j] << " " ;
    cout << endl;
    /*
        NetworkAddress addr2("");
        NetworkAddress addr3("boo:8080");
        NetworkAddress addr4("192.168.5.11:8080");
     
        cout << "NetworkAddress addr1(\"192.168.5.11\", 8080);" << endl;
        cout << "NetworkAddress addr2(\"boo\", 8080);" << endl;
        cout << "NetworkAddress addr3(\"boo:8080\");" << endl;
        cout << "NetworkAddress addr4(\"192.168.5.11:8080\");" << endl << endl;
     
        cout << "addr1.getHostName()= " << addr1.getHostName() << endl;
        cout << "addr2.getHostName()= " << addr2.getHostName() << endl;
        cout << "addr3.getHostName()= " << addr3.getHostName() << endl;
        cout << "addr4.getHostName()= " << addr4.getHostName() << endl << endl;
     
        cout << "addr1.getIpName()= " << addr1.getIpName() << endl;
        cout << "addr2.getIpName()= " << addr2.getIpName() << endl;
        cout << "addr3.getIpName()= " << addr3.getIpName() << endl;
        cout << "addr4.getIpName()= " << addr4.getIpName() << endl << endl;
     
        cout << "addr1.getIp4Address()= " << dec << addr1.getIp4Address()
             << " HEX=" << hex << addr1.getIp4Address() << endl;
        cout << "addr2.getIp4Address()= " << dec << addr2.getIp4Address() 
             << " HEX=" << hex << addr2.getIp4Address() << endl;
        cout << "addr3.getIp4Address()= " << dec << addr3.getIp4Address() 
             << " HEX=" << hex << addr3.getIp4Address() << endl;
        cout << "addr4.getIp4Address()= " << dec << addr4.getIp4Address() 
             << " HEX=" << hex << addr4.getIp4Address() << endl << endl;
     
        cout << dec;
     
        cout << "addr1.getPort()= " << addr1.getPort() << endl;
        cout << "addr2.getPort()= " << addr2.getPort() << endl;
        cout << "addr3.getPort()= " << addr3.getPort() << endl;
        cout << "addr4.getPort()= " << addr4.getPort() << endl << endl;
     
        cout << "string(addr1)= " << string(addr1) << endl;
        cout << "string(addr2)= " << string(addr2) << endl;
        cout << "string(addr3)= " << string(addr3) << endl;
        cout << "string(addr4)= " << string(addr4) << endl << endl;
     
        cout << "ostream addr1= " << addr1 << endl;
        cout << "ostream addr2= " << addr2 << endl;
        cout << "ostream addr3= " << addr3 << endl;
        cout << "ostream addr4= " << addr4 << endl << endl;
    */

}
