
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
static const char* const tClient_cxx_Version = 
"$Id: tClient.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "Application.hxx"
#include "UDPSocket.hxx"
#include "TCPClientSocket.hxx"
#include "TPKTClientSocket.hxx"
#include "IPAddress.hxx"
#include "VLog.hxx"

#include <unistd.h>


using namespace Vocal;
using namespace Vocal::Threads;
using namespace Vocal::Transport;
using namespace Vocal::Logging;


class tClient : public Application
{
    public:
    	tClient() {}
	~tClient() {}

    	ReturnCode  	    init(int, char **, char **);
	void	    	    uninit();
	
    	ReturnCode  	    run();	
};

class tClientCreator : public ApplicationCreator
{
    Application * create() { return ( new tClient ); }
};

ReturnCode
tClient::init(int argc, char ** argv, char ** arge)
{
    if ( argc > 1 )
    {
        VLog::init(LOG_DEBUG, argv[1]);
    }
    else
    {
    	VLog::init(LOG_DEBUG);
    }

//    VLog::on(LOG_DEBUG);
//    VLog::on(LOG_INFO);
    if ( argc > 2 )
    {
//        VLog::on(LOG_TRACE);
    }
    return ( SUCCESS );
}

void
tClient::uninit()
{
    VLog::uninit();
}

ReturnCode
tClient::run()
{
    try
    {
    	IPAddress           udpEcho("192.168.5.23", 7777),
    	    	    	    tcpEcho("192.168.5.23", 7778),
    	    	    	    tpktEcho("192.168.5.23", 7779);
	
	cout << "udpEcho address: " << udpEcho << endl;
	cout << "tcpEcho address: " << tcpEcho << endl;
	cout << "tpktEcho address: " << tpktEcho << endl;

  	UDPSocket           udpSock;
	TCPClientSocket     tcpSock;
	TPKTClientSocket    tpktSock;

	udpSock.setBlocking();
	tcpSock.setBlocking();
    	tpktSock.setBlocking();
	
	udpSock.connect(udpEcho);
	tcpSock.connect(tcpEcho);
	tpktSock.connect(tpktEcho);

    	char	    *	sendMessage[10] =
	{
	    "message 0",
	    "message 1",
	    "message 2",
	    "message 3",
	    "message 4",
	    "message 5",
	    "message 6",
	    "message 7",
	    "message 8",
	    "message 9"
	};
	
	for ( int i = 0; i < 10; i ++ )
	{
    	    size_t messageLength = strlen(sendMessage[i]);
	    
	    cout << "Sending message: " << sendMessage[i]
		 << ", size: " << messageLength
		 << ", to: " << udpEcho << endl;
    	    
    	    udpSock.send(sendMessage[i]);

	    cout << "Sending message: " << sendMessage[i]
		 << ", size: " << messageLength
		 << ", to: " << tcpEcho << endl;
    	    
    	    tcpSock.send(sendMessage[i]);

	    cout << "Sending message: " << sendMessage[i]
		 << ", size: " << messageLength
		 << ", to: " << tpktEcho << endl;
    	    
    	    tpktSock.send(sendMessage[i]);

    	    string  	* recvMessage = new string(8191, ' ');
	    
	    int bytesReceived = udpSock.receive(*recvMessage);

	    cout << "Received message: " << recvMessage->c_str()
		 << ", size: " << recvMessage->size() << endl;

    	    delete recvMessage;

    	    recvMessage = new string(8191, ' ');
	    
	    bytesReceived = tcpSock.receive(*recvMessage);

	    cout << "Received message: " << recvMessage->c_str()
		 << ", size: " << recvMessage->size() << endl;

    	    delete recvMessage;
	    
    	    u_int16_t	msgSize = tpktSock.receiveTPKTHeader();

    	    recvMessage = new string(msgSize, ' ');

    	    bytesReceived = tpktSock.receive(*recvMessage);

    	    assert(msgSize == bytesReceived);
	    
	    cout << "Received message: " << recvMessage->c_str()
		 << ", size: " << recvMessage->size() << endl;

    	    delete recvMessage;

    	    sleep(1);
	}
    }
    catch ( ... )
    {
    	cout << "caught error" << endl;
    }
    
    return ( 0 );
}

int main(int argc, char ** argv, char ** arge)
{
    tClientCreator  creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}
