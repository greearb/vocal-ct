
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
static const char* const tSocket_cxx_Version = 
"$Id: tSocket.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "Application.hxx"
#include "Socket.hxx"
#include "VLog.hxx"
#include "IPAddress.hxx"
#include "UDPSocket.hxx"
#include <unistd.h>
#include <stdio.h>
#include <iomanip>
#include <cerrno>
#include <cstring>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <list>
#include "MACAddress.hxx"

//#include "PollFifo.hxx"


using namespace Vocal;
using namespace Vocal::Transport;
using namespace Vocal::Logging;


class TestSocket : public Socket
{
    public:
    
	TestSocket(
	    TransportAddress & localAddr,
	    const SocketType &);

	~TestSocket();
};


TestSocket::TestSocket(
    TransportAddress & localAddr,
    const SocketType & socketType
)
    :	Socket(localAddr, socketType)
{
}


TestSocket::~TestSocket()
{
}


class tSocket : public Application
{
    public:
    	tSocket() {}
	~tSocket() {}

    	ReturnCode  	    init(int, char **, char **);
	void	    	    uninit();
	
    	ReturnCode  	    run();	
};



ostream &   operator<<(ostream & out, u_int8_t * buffer)
{
    out << "0x";
    for ( int i = 0; i < 26; i++ )
    {
    	out << setw(2) << setfill('0') << hex << (unsigned)(buffer[i]) << dec;
    }
    return ( out );
}


class tSocketCreator : public ApplicationCreator
{
    Application * create() { return ( new tSocket ); }
};

ReturnCode
tSocket::init(int argc, char ** argv, char ** arge)
{
    if ( argc > 1 )
    {
        VLog::init(LOG_DEBUG, argv[1]);
    }
    else
    {
    	VLog::init(LOG_DEBUG);
    }

    VLog::on(LOG_DEBUG);
    VLog::on(LOG_INFO);
    if ( argc > 2 )
    {
        VLog::on(LOG_TRACE);
    }
    return ( SUCCESS );
}

void
tSocket::uninit()
{
    VLog::uninit();
}

ReturnCode
tSocket::run()
{
    vector<int>     v(1024);

    cout << "size: " << v.size() << ", capacity: " << v.capacity() << endl;

    v.resize(10);
    v[0] = 0;
    v[1] = 1;
    v[2] = 2;
    v[3] = 3;
    v[9] = 9;

    v.resize(20);

    cout << "size: " << v.size() << ", capacity: " << v.capacity() << endl;
    
    for ( unsigned int i = 0; i < v.size(); i++ )
    {
    	cout << v[i] << ' ';
    }
    cout << endl;
    

    u_int8_t	feh[2];
    
    memset(feh, 2, 0);
    
    unsigned short *	us_feh = (unsigned short *)&feh;
    
    *us_feh = htons(0xABCD);
    
    cout << setw(2) << setfill(' ') << hex << (u_int16_t)(feh[0]) 
    	 << setw(2) << setfill(' ') << hex << (u_int16_t)(feh[1]) 
    	 << dec << endl;
    

    MACAddress	macAddr;
    
    cout << "macAddr high: " << (void *)(macAddr.high()) 
    	 << " low: " << (void *)(macAddr.low()) << endl;
	 
    tms     	startTimeBuf;
    clock_t 	start = times(&startTimeBuf);

    sockaddr_in     ip0, ip1, ip2;
    sockaddr	*   sa0 = (sockaddr *)&ip0,
    	    	*   sa1 = (sockaddr *)&ip1,
    	    	*   sa2 = (sockaddr *)&ip2;

    socklen_t	    sa_len = sizeof(sockaddr_in);
    		
    sa0->sa_family = sa1->sa_family = AF_INET;
    ip0.sin_addr.s_addr = ip1.sin_addr.s_addr = htonl(INADDR_ANY);
    ip0.sin_port = htons(11111);
    ip1.sin_port = htons(11112);
    
    int fd0 = socket(AF_INET, SOCK_STREAM, 0),
    	fd1 = socket(AF_INET, SOCK_STREAM, 0),
	fd2;

    if  (   bind(fd0, sa0, sa_len) < 0 
    	||  bind(fd1, sa1, sa_len) < 0
	||  listen(fd0, 0) < 0
	||  connect(fd1, sa0, sa_len) < 0
	)
    {
    	int error = errno;
    	cerr << "ack: " << strerror(error) << endl;
    }
    else
    {
    	cerr << "connected" << endl;
	socklen_t len = sa_len;
	
	fd2 = accept(fd0, sa2, &len);
	if ( fd2 < 0 )
	{
	    cerr << "accept ack: " << strerror(errno) << endl;
	}
    }
    
    close(fd0);
    close(fd1);
    close(fd2);

    string  	    sendBuffer("test");
    IPAddress	    interruptorAddr(44445);
    UDPSocket	    interruptor(interruptorAddr, "Interruptor");

    interruptor.sendTo(sendBuffer, interruptorAddr);

    fd0 = interruptor.getFD();
            
    fd_set	readFds;
    FD_ZERO(&readFds);
    FD_SET(fd0, &readFds);

    timeval to;

    to.tv_sec = 10;
    to.tv_usec = 0;

    if  ( 	select(fd0+1, &readFds, 0, 0, &to) < 0 
	||	!FD_ISSET(fd0, &readFds)
	)
    {
	if ( errno )
	{
            cerr << "interruptor ack: " << strerror(errno) << endl;
	}
    }
    else
    {
	string buffer(32, ' ');
	IPAddress   remoteAddr;

	interruptor.receiveFrom(buffer, remoteAddr);
	
	if ( buffer != sendBuffer )
	{
            cerr << "interruptor not the same" 
	    	 << ": sendBuffer: " << sendBuffer
		 << ", recvBuffer: " << buffer
	    	 << endl;
	}
    }
    cerr << "interruptor works." << endl;

    vector<int>     vfeh;
    
    vfeh.insert(vfeh.end(), 2);
    vfeh.insert(vfeh.end(), 4);
    vfeh.insert(vfeh.end(), 6);
    
    cout << vfeh[0] << ", " << vfeh[1] << ", " << vfeh[2] << endl;

    sleep(2);

    tms     stopTimeBuf    ;
    clock_t stop = times(&stopTimeBuf);

    cout << "start: " << start 
    	 << ", stop: " << stop
    	 << ", CLK_TCK: " << CLK_TCK
    	 << ", run time in hundredths of a sec: " << (stop - start) 
    	 << ", in sec: " << (((double)(stop-start))/(double)CLK_TCK)
    	 << ", start user time: " << startTimeBuf.tms_utime
    	 << ", start system time: " << startTimeBuf.tms_stime
    	 << ", start child user time: " << startTimeBuf.tms_cutime
    	 << ", start child system time: " << startTimeBuf.tms_cstime
    	 << ", stop user time: " << stopTimeBuf.tms_utime
    	 << ", stop system time: " << stopTimeBuf.tms_stime
    	 << ", stop child user time: " << stopTimeBuf.tms_cutime
    	 << ", stop child system time: " << stopTimeBuf.tms_cstime
    	 << endl;


    list<int>	lfeh;
    lfeh.push_back(2);
    lfeh.push_back(4);
    lfeh.push_back(6);
    lfeh.push_back(8);
    lfeh.push_back(10);
    lfeh.push_back(14);
    
    list<int>::iterator it;

    for ( it = lfeh.begin(); it != lfeh.end(); it++ )
    {
    	cout << *it << " ";
    }
    cout << endl;
    
    for ( it = lfeh.begin(); it != lfeh.end(); it++ )
    {
    	if ( *it > 7 )
	{
	    lfeh.insert(it, 7);
	    break;
	}
    }
    for ( it = lfeh.begin(); it != lfeh.end(); it++ )
    {
    	cout << *it << " ";
    }
    cout << endl;
    
    for ( it = lfeh.begin(); it != lfeh.end(); it++ )
    {
    	if ( *it > 7 )
	{
	    lfeh.insert(it, 7);
	    break;
	}
    }
    for ( it = lfeh.begin(); it != lfeh.end(); it++ )
    {
    	cout << *it << " ";
    }
    cout << endl;

    for ( it = lfeh.begin(); it != lfeh.end(); it++ )
    {
    	if ( *it > 15 )
	{
	    lfeh.insert(it, 15);
	    break;
	}
    }
    if ( it == lfeh.end() )
    {
    	lfeh.push_back(15);
	cout << "On the end" << endl;
    }
    for ( it = lfeh.begin(); it != lfeh.end(); it++ )
    {
    	cout << *it << " ";
    }
    cout << endl;



    IPAddress	    ipAddr(80);
    SocketType	    type(SOCK_STREAM);
    try
    {
        TestSocket     sock(ipAddr, type), sock_fail(ipAddr, type);
    }
    catch ( ... )
    {
    	cout << "caught it" << endl;
    }

    try
    {
        UDPSocket       udpSock;

    	IPAddress       echo("127.0.0.1", 7777),
			fromAddr;

    	char	    	sendMessage[8192];
	    	    	//recvMessage[8192];

    	u_int8_t    	binarySendMessage[26];
	
	for ( int i = 0; i < 26; i++ )
	{
	    binarySendMessage[i] = 'a' + i;
	}
	
	cout << "binary message: " << binarySendMessage << endl;
	
	udpSock.setBlocking();

    	int count = 0;
	
    	while ( count++ < 100 )
	{
    	    size_t sendMessageLength = 26;

	    if ( count % 4 )
	    {
            	if ( fgets(sendMessage, 8191, stdin) == 0 )
    		{
    	    	    return ( 0 );
		}
    	    	size_t sendMessageLength = strlen(sendMessage);
	    
		cout << "Sending message: " << sendMessage 
		     << ", size: " << sendMessageLength
		     << ", to: " << echo << endl;
    	    
    	    	udpSock.sendTo(sendMessage, echo);
	    }
	    else
	    {
	    	udpSock.sendTo(binarySendMessage, sendMessageLength, echo);
	    }

    	    string  	* recvMessage = new string(8191, ' ');
	    
	    int bytesReceived = udpSock.receiveFrom(*recvMessage, fromAddr);

	    cout << "Received message: " << recvMessage->c_str()
		 << ", size: " << recvMessage->size() 
		 << ", bytes reported = " << bytesReceived
		 << ", from: " << fromAddr << endl;

    	    delete recvMessage;
	}
    }
    catch ( ... )
    {
    	cout << "caught UDP error" << endl;
    }
    
    return ( 0 );
}

int main(int argc, char ** argv, char ** arge)
{
    tSocketCreator  creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}
