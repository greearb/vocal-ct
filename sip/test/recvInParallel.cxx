
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
static const char* const recvInviteVersion =
    "$Id: recvInParallel.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <pthread.h>

#include "cpLog.h"
#include "Sptr.hxx"
#include "Data.hxx"
#include "VovidaSipStack.hxx"
#include "InviteMsg.hxx"
#include "AckMsg.hxx"
#include "TimeFunc.hxx"
#include "ByeMsg.hxx"
#include "BaseUrl.hxx"
#include "VThread.hxx"
#include "VMutex.h"


using namespace Vocal;
    

    
//global mutex
VMutex fifo_mutex;

//FIFO to store messages.
Fifo< Sptr<SipMsg> > msg_queue;



void * sendResp(void *stack);
void * recvReq(void *stack);
    
///
int
main( int argc, char* argv[] )
{
    if ( argc < 2)
    {
        cerr << "Usage: "
        << argv[0]
        << " <local_SIP_port>"
        << endl;
        exit( -1 );
    }

    Data logFile = "/tmp/itr";

    cpLogOpen( logFile.logData() );

    cpLogSetPriority(LOG_DEBUG_STACK);


    cout << "\t<<< Recieve Invite in parallel test >>>\n" << endl;
    cout << "     Local SIP Port : " << argv[1] << endl;
    cout << "      Return Status : 200 "  << endl;

    pthread_t sendThread;
    pthread_t recvThread;
    SipTransceiverFilter* stack = new SipTransceiverFilter( argv[0], atoi( argv[1] ) );

    stack->setRandomLosePercent(0);

    pthread_create(&sendThread, NULL, sendResp,(void*)stack);
    pthread_create(&recvThread, NULL, recvReq,(void*)stack);

    pthread_join(sendThread, NULL);
    pthread_join(recvThread, NULL);
}



void * recvReq(void * s)
{
    SipTransceiverFilter* stack = (SipTransceiverFilter *)s;

    Sptr < SipMsgQueue > sipRcv;
    Sptr < SipMsg > sipMsg;

    while ( 1 )
    {
        cout << "Received Request" << endl;
        sipRcv = stack->receive();
       
        
        if ( sipRcv != 0 )
        {
	
            sipMsg = sipRcv->back();
            if ( sipMsg.getPtr() != 0 )
            {
                //put into FIFo

                //fifo_mutex.lock();
                msg_queue.add(sipMsg);
               // fifo_mutex.unlock();
                                         
            }
        }
        //sleep(4);
    }//end while
    if (stack)
    {
        delete stack;
    }
    
    return 0;
}



void * sendResp(void * s)
{
    SipTransceiverFilter* stack = (SipTransceiverFilter *)s;

    while(1)
    {
        cout << " Sending Response" << endl;
        Sptr < InviteMsg > inviteMsg;
        Sptr < AckMsg > ackMsg;
        Sptr < ByeMsg > byeMsg;
        Sptr <SipMsg> sipMsg;
    

       //read from FIFO and process.
       //fifo_mutex.lock();
       sipMsg = msg_queue.getNext();
      // fifo_mutex.unlock();
 
        //Now, we have a sptr in sipMsg
        inviteMsg.dynamicCast( sipMsg );
        if ( inviteMsg == 0 )
        {
	    ackMsg.dynamicCast( sipMsg );
	    if ( ackMsg == 0 )
	    {
	        byeMsg.dynamicCast( sipMsg);
	        if (byeMsg == 0)
	        {
		    cpLog( LOG_ERR, "Not an Invite or Ack message" );
	        }
	        else
	        {
		    StatusMsg statusMsg( *byeMsg, 200 );
		    stack->sendReply( statusMsg );
	        }
	    }
	    else
	    {
	        cpLog( LOG_DEBUG, "ReceivedAck message, Do nothing" );
            }
        }
        else 
        {
	    int status = 200;

    	    StatusMsg statusMsg( *inviteMsg, status );
	    {
	        // Add myself to the contact
	        Sptr <BaseUrl> myUrl;
	        myUrl = inviteMsg->getRequestLine().getUrl();

	        SipContact me;
	        me.setUrl( myUrl );
	        statusMsg.setNumContact( 0 );    // Clear
	        statusMsg.setContact( me );
	    }


	    stack->sendReply( statusMsg );
        }

    }//end while
    if (stack)
    {
        delete stack;
    }
    
    return 0;
}
