
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
static const char* const test_udp_receive_cxx_Version =
    "$Id: test_udp_receive.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "UdpStack.hxx"
#ifdef __vxworks
#include "resolvLib.h"
#endif


int main()
{

#ifdef __vxworks
    resolvInit("192.168.5.254", "private.vovida.com", 0);
#endif

    //    NetworkAddress target("192.168.5.99", 5000);
    NetworkAddress target("carver", 5009);
    NetworkAddress target1("localhost", 5010);

    //    UdpStack stack(&target, 5000, 5000, recvonly);
    UdpStack stack(NULL, 9000, 9000, sendrecv);
    stack.setDestination(&target);
    //    stack.connectPorts();

    /*
        stack.setDestination(&target1);
        stack.connectPorts();
    */
    //   stack.disconnectPorts(); // ?? ports are still connected ??


    char buf[256];
    NetworkAddress sender;

    //    while(1) {
    //    int bytesRead = stack.receive(buf, 256);
    int bytesRead = stack.receiveFrom(buf, 256, &sender);

    cout << "bytes read=" << bytesRead << endl;
    cout << buf << endl;
    //    }

    cout << sender << endl;

    cout << endl;

    return 0;
}
