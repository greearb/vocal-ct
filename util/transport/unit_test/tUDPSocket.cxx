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


static const char* const tUDPSocket_cxx_Version = 
    "$Id: tUDPSocket.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "Application.hxx"
#include "UDPSocket.hxx"
#include "IPAddress.hxx"


using namespace Vocal;
using namespace Vocal::Transport;

class TestApplication : public Application
{
    public:
    	ReturnCode  	init(int argc, char ** argv, char ** arge);
	void	    	uninit();
    	ReturnCode  	run();
};

class TestApplicationCreator : public ApplicationCreator
{
    Application * create() { return ( new TestApplication ); }
};


ReturnCode  
TestApplication::init(int argc, char ** argv, char ** arge)
{
    return ( SUCCESS );
}


void
TestApplication::uninit()
{
}


int main(int argc, char ** argv, char ** arge)
{
    TestApplicationCreator  creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}


ReturnCode TestApplication::run()
{
    IPAddress   localAddr(1812),
                remoteAddr(1813);

    cout << "Local Address:\t" << localAddr << endl;
    cout << "Remote Address:\t" << remoteAddr << endl;
        
    char        buffer[11] = "testing123";

    UDPSocket   localUdpSocket(localAddr, remoteAddr);
    UDPSocket   remoteUdpSocket(remoteAddr);

    cout << "Local UDP Socket:\t" << localUdpSocket << endl;
    cout << "Remote UDP Socket:\t" << remoteUdpSocket << endl;
    
    int sent = localUdpSocket.send((u_int8_t*)buffer, 11);

    cout    << "Local sent buffer:\t" << buffer 
            << ", Size: " << sent 
            << ", To: " << remoteAddr
            << endl;
    
    u_int8_t    remote_buffer[32];
    IPAddress   remoteFromAddr;

    int received = remoteUdpSocket.receiveFrom(remote_buffer, 32, remoteFromAddr);
    
    cout    << "Remote received buffer: " << (char *)remote_buffer
            << ", Size: " << received << ", From: " << remoteFromAddr
            << endl;

    sent = remoteUdpSocket.sendTo(remote_buffer, received, remoteFromAddr);

    cout    << "Remote sent buffer:\t" << buffer 
            << ", Size: " << sent
            << ", To: " << remoteFromAddr
            << endl;

    u_int8_t    local_buffer[32];

    received = localUdpSocket.receive(local_buffer, 32);
    
    cout    << "Local received buffer:\t" << (char *)local_buffer
            << ", Size: " << received << ", From: " << remoteAddr
            << endl;

    cout << "Local UDP Socket:\t" << localUdpSocket << endl;
    cout << "Remote UDP Socket:\t" << remoteUdpSocket << endl;
    
    return ( SUCCESS );
}
