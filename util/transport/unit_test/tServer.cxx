
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
static const char* const tServer_cxx_Version = 
"$Id: tServer.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "Application.hxx"
#include "Socket.hxx"
#include "VLog.hxx"
#include "IPAddress.hxx"
#include "UDPSocket.hxx"
#include "StreamClientSocket.hxx"
#include "TCPServerSocket.hxx"
#include "TPKTServerSocket.hxx"
#include "Thread.hxx"


using namespace Vocal;
using namespace Vocal::Threads;
using namespace Vocal::Logging;
using namespace Vocal::Transport;

class UDPServer : public Runnable
{
    public:
    	UDPServer(u_int16_t port) : echo_(port), udpServer_(0), count_(0)
	{   udpServer_ = new UDPSocket(echo_); }
	
	~UDPServer() 
	{   delete udpServer_; }
	    
    	ReturnCode  	    run();    
    
    private:
    	IPAddress   	    echo_;
        UDPSocket   	*   udpServer_;
	unsigned int 	    count_;
};

class TCPServer : public Runnable
{
    public:
    	TCPServer(u_int16_t port) : echo_(port), tcpServer_(0)
	{   tcpServer_ = new TCPServerSocket(echo_);   }
	    
	~TCPServer() 
	{   delete tcpServer_;	}
	
	ReturnCode  	    run();
	
    private:
    	IPAddress   	    echo_;
	TCPServerSocket *   tcpServer_;
};

class TPKTServer : public Runnable
{
    public:
    	TPKTServer(u_int16_t port) : echo_(port), tpktServer_(0)
	{   tpktServer_ = new TPKTServerSocket(echo_);   }
	    
	~TPKTServer() 
	{   delete tpktServer_;	}
	
	ReturnCode  	    run();
	
    private:
    	IPAddress   	    echo_;
	TPKTServerSocket *  tpktServer_;
};

class tServer : public Application
{
    public:
    	tServer() {}
	~tServer() {}

    	ReturnCode 	    init(int, char **, char **);
	void	    	    uninit();
	
    	ReturnCode  	    run();	
};

ReturnCode
UDPServer::run()
{
    try
    {
	udpServer_->setBlocking();

	char 	    	recvMessage[8192];

    	while ( count_ < 10 )
	{
            cout << "Receiving udp message..." << endl;	

    	    IPAddress	    fromAddr;
	    
    	    int bytesReceived = udpServer_->receiveFrom(recvMessage, 8192, fromAddr);

	    cout << "Received message: " << recvMessage
		 << ", size: " << bytesReceived
		 << ", on: " << *udpServer_
		 << ", from: " << fromAddr << endl;

    	    udpServer_->sendTo(recvMessage, fromAddr);
	    
	    count_++;
	}
    }
    catch ( ... )
    {
    	cout << "caught UDP error" << endl;
	return ( 1 );
    }
    
    return ( 0 );
}

ReturnCode
TCPServer::run()
{
    try
    {
	tcpServer_->setBlocking();

    	Sptr<TCPClientSocket>	tcpSocket = tcpServer_->accept();
	
	char 	    	recvMessage[8192];

    	while ( 1 )
	{
            cout << "Receiving tcp message..." << endl;	

    	    int bytesReceived = tcpSocket->receive(recvMessage, 8192);

	    cout << "Received message: " << recvMessage
		 << ", size: " << bytesReceived 
		 << ", on : " << *tcpSocket << endl;

    	    tcpSocket->send(*recvMessage ? recvMessage : "No message received.");
	}
    }
    catch ( ... )
    {
    	cout << "caught TCP error" << endl;
	return ( 1 );
    }
    
    return ( 0 );
}

ReturnCode
TPKTServer::run()
{
    try
    {
	tpktServer_->setBlocking();

    	Sptr<TPKTClientSocket>    tpktSocket = tpktServer_->accept();
	
	char 	    	recvMessage[8192];

    	while ( 1 )
	{
            cout << "Receiving tpkt message..." << endl;	

    	    u_int16_t	pktSize = tpktSocket->receiveTPKTHeader();

    	    assert( pktSize < 8192 );
	    	    
    	    int bytesReceived = tpktSocket->receive(recvMessage, 8192);

	    cout << "Received message: " << recvMessage
		 << ", size: " << bytesReceived 
		 << ", length reported: " << pktSize
		 << ", on : " << *tpktSocket << endl;

    	    tpktSocket->send(*recvMessage ? recvMessage : "No message received.");
	}
    }
    catch ( ... )
    {
    	cout << "caught TPKT error" << endl;
	return ( 1 );
    }
    
    return ( 0 );
}

class tServerCreator : public ApplicationCreator
{
    Application * create() { return ( new tServer ); }
};

ReturnCode
tServer::init(int argc, char ** argv, char ** arge)
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
tServer::uninit()
{
    VLog::uninit();
}

ReturnCode
tServer::run()
{
     UDPServer	udpServer(7777);
     Thread  	udpServerThread(udpServer, "udpServerThread");
     
     TCPServer	tcpServer(7778);
     Thread  	tcpServerThread(tcpServer, "tcpServerThread");

    TPKTServer 	tpktServer(7779);
    Thread  	tpktServerThread(tpktServer, "tpktServerThread");
    
    ReturnCode	rc = SUCCESS;
    
    if  (   (rc = udpServerThread.join())   != SUCCESS 
    	||  (rc = tcpServerThread.join())   != SUCCESS 
    	||  (rc = tpktServerThread.join())  != SUCCESS 
	)
    {
    	return ( rc );
    }
    
    return ( SUCCESS );
}

int main(int argc, char ** argv, char ** arge)
{
    tServerCreator  creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}
