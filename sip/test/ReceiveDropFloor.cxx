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


static const char* const ReceiveDropFloor_cxx_Version = 
    "$Id: ReceiveDropFloor.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "UdpStack.hxx"
#include "VThread.hxx"
#include "Fifo.h"
#include "SipMsg.hxx"
#include "TransceiverSymbols.hxx"
#include "DurationStatistic.hxx"
#include "PerformanceDb.hxx"
#include "Data.hxx"

using namespace Vocal;
using Vocal::Statistics::DurationStatistic;
using Vocal::Statistics::StatisticsDb;

///
class ReceiveDropFloor
{
public:

    ///
    ReceiveDropFloor(Fifo<int>* lock, int count, int port = SIP_PORT);
	~ReceiveDropFloor();
private:
     ///
     static bool Udpretransmitoff;
     ///
     static int  Udpretransmitimeinitial;
     ///
     static int Udpretransmittimemax;
     ///
     int randomLosePercent;
     ///
     UdpStack udpStack;
     ///
     VThread receiveThread;
     /// 
     VMutex shutdownMutex;
     ///
     VMutex fifoEventMapMutex;
   
     ///
     bool shutdown;                                       
    
     //FifoEventId addDelayMs(const Msg& msg, const int delayMs);
     //TransactionMap tmap;    
     ///
     static void* sendThreadWrapper(void *p);
     /// 
     static void* rcvThreadWrapper(void *p);
     ///    
     void* receiveMain();
     ///    
     void* sendMain();
     ///        
     bool retransmission(Sptr<SipMsg> sipMessage);
     ///    
     void getHostPort(Sptr<SipMsg> sipMessage, Data* host, int* port);

	Fifo<int>* myLock;
	///

	int myCount;
};


ReceiveDropFloor::ReceiveDropFloor(Fifo<int>* lock, int count, int port /*default argument*/ )
    :
    randomLosePercent(0),
    udpStack( NULL, port ),
    shutdown(false),
    myLock(lock),
    myCount(count)
{
    receiveThread.spawn(rcvThreadWrapper, this);
}   


ReceiveDropFloor::~ReceiveDropFloor() 
{      
    shutdownMutex.lock();
    shutdown = true;
    shutdownMutex.unlock();
    receiveThread.join();
}



void*
ReceiveDropFloor::rcvThreadWrapper(void *p) 
{
    return  static_cast<ReceiveDropFloor*>(p)->receiveMain();
}       


void*
ReceiveDropFloor::receiveMain()
{      
    fd_set netFD;
    NetworkAddress sender;
    while (1)
    {
	if(myCount <= 0)
	{
	    shutdown = true;
	}
        FD_ZERO (&netFD);
        int maxfd = udpStack.getSocketFD();
        int len = 0;
        int newlen = 0;	
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;
        FD_SET(maxfd,&netFD);

        int selret = select(maxfd+1, &netFD, NULL, NULL, &timeout);
        if ( selret > 0) 
        {
            static const string::size_type MTU = 3600;
            char buf[MTU+1];
            buf[0] = '\0';
            bool readfd = FD_ISSET(udpStack.getSocketFD(), &netFD);
            
            if (readfd)
            { 
                len = udpStack.receiveFrom( buf,MTU , &sender );
                buf[len] = '\0';
            }
            
            newlen = strlen(buf);
            
            if (( newlen == 0) || ( newlen < 0 ))
            {
                return 0;
            }
            else
            {
                assert( newlen > 0);
                buf[newlen] = '\0';
                
                Data data(buf, newlen);

                cpLog( LOG_DEBUG_STACK, "Received UDP Message \n%s" , 
		       data.logData());

		myLock->add(1);
	    }
        }
        if ( selret < 0) 
        {
            if ( errno != EBADF && errno != EINTR )
            {
        	cpLog ( LOG_ALERT, 
			"select () failed, reason: %s", strerror(errno));
                assert(0);
            }
       }
       if (selret == 0) 
       {
	    shutdownMutex.lock();
	    bool shouldShutdown = shutdown;
	    shutdownMutex.unlock();
	    if(shouldShutdown)
	    {
		return 0;
	    }
       }
    }
    myLock->add(1);
    return 0;
}


int main()
{
    Fifo<int> lock;
    int i;
    StatisticsDb db;

    DurationStatistic stat(db, Data("receiveDropFloor"));

    ReceiveDropFloor start(&lock, 101);
    lock.getNext();
    stat.start();
    for(i = 0; i < 100 ; i++) {
	lock.getNext();
    }
    stat.stop();
    stat.record();

    cout << db << endl;

    return 0;
}
