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
    "$Id: tUDPClientServer.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include "Application.hxx"
#include "UDPSocket.hxx"
#include "IPAddress.hxx"
#include "Poll.hxx"
#include "SocketOptions.hxx"
#include "VCondition.h"
#include <string>


using namespace Vocal;
using namespace Vocal::Transport;

class TestApplication : public Application
{
    public:
        TestApplication();
        
    	ReturnCode  	init(int argc, char ** argv, char ** arge);
	void	    	uninit();
    	ReturnCode  	run();

    private:

        ReturnCode      runServer();
        ReturnCode      runClient();
        
        bool            server;
        string          remoteIPAddress;
        
};

class TestApplicationCreator: public ApplicationCreator
{
    Application * create() { return ( new TestApplication ); }
};


TestApplication::TestApplication()
    :   server(true),
        remoteIPAddress("127.0.0.1")
{
}

ReturnCode  
TestApplication::init(int argc, char ** argv, char ** arge)
{
    
    if ( argc > 1 )
    {
        server = false;
        remoteIPAddress.assign(argv[1]);
    }
        
    return ( SUCCESS );
}


void
TestApplication::uninit()
{
}


int main(int argc, char ** argv, char ** arge)
{
    TestApplicationCreator creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}


ReturnCode TestApplication::run()
{
    if ( server )
    {
        return ( runServer() );
    }
    return ( runClient() );
}

ReturnCode TestApplication::runServer()
{
    IPAddress   serverAddr(1813);

    UDPSocket   serverSocket(serverAddr);
                
    cout << "Server Address:\t" << serverAddr << endl;
    cout << "Server Socket:\t" << serverSocket << endl;

    bool    done = false;
    
    while ( !done )
    {
        char        buffer[32];
        IPAddress   remoteAddr;

        int received = serverSocket.receiveFrom(buffer, 
                                                32, 
                                                remoteAddr);
        buffer[received] = '\0';

        cout    << "Server received buffer: " << buffer
                << ", Size: " << received << ", From: " << remoteAddr
                << endl;

        int sent = serverSocket.sendTo(buffer, remoteAddr);

        cout    << "Server sent buffer:\t" << buffer 
                << ", Size: " << sent
                << ", To: " << remoteAddr
                << endl;

        cout << "Server Socket:\t" << serverSocket << endl;
    
        if ( *buffer == 'q' )
        {
            done = true;
        }
    }
    
    return ( SUCCESS );
}

ReturnCode TestApplication::runClient()
{
    IPAddress   localAddr(1812),
                remoteAddr;

    remoteAddr.setIP(remoteIPAddress.c_str());
    UDPSocket   clientSocket(localAddr, remoteAddr);
    SocketOptions   clientSockOptions(clientSocket);
    
    clientSockOptions.bsdCompatible();

        
    cout << "Local Address:\t" << localAddr << endl;
    cout << "Remote Address:\t" << remoteAddr << endl;
    cout << "Client Socket:\t" << clientSocket << endl;
    
    char    buffer[6][11] = 
    {
        "testing123",
        "testing456",
        "testing789",
        "testingABC",
        "testingDEF",
        "quit"
    };

    Poll    poll;

    poll.registerFD(clientSocket, Poll::Incoming | Poll::Priority);
    
    for ( int i = 0; i < 6; i++ )
    {
        int sent = clientSocket.send(buffer[i]);

        cout    << "Client sent buffer:\t" << buffer[i]
                << ", Size: " << sent 
                << ", To: " << remoteAddr
                << endl;

        u_int8_t    receive_buffer[32];

        int fdsActive = poll.poll();
        
        assert( fdsActive == 1 );

        int received = clientSocket.receive(receive_buffer, 32);

        receive_buffer[received] = '\0';
    
        cout    << "Client received buffer:\t" << (char *)receive_buffer
                << ", Size: " << received << ", From: " << remoteAddr
                << endl;

        cout << "Client Socket:\t" << clientSocket << endl;
    }
    
    try 
    {
        while ( true )
        {
            clientSocket.send("testing123");
            cout << "Sending..." << endl;
            vusleep(250);
        }
    }
    catch ( VException & )
    {
        cout << "Other side is gone. This is a good thing." << endl;
    }
    
    return ( SUCCESS );
}
