
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
    "$Id: perfrecvInvite.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <stdlib.h>
#include <unistd.h>
#include <cstdlib>
#include <fstream>

#include "Sptr.hxx"
#include "Data.hxx"
#include "VovidaSipStack.hxx"
#include "InviteMsg.hxx"
#include "PerfDuration.hxx"
#include "PerformanceDb.hxx"    
#include "TickCount.hxx"
#include "Statistic.hxx"
#include "SystemInfo.hxx"

    
using namespace Vocal;
using Vocal::Statistics::PerfDuration;
using Vocal::Statistics::PerformanceDb;    
using Vocal::Statistics::TickCount;
using Vocal::Statistics::Statistic;
    
///
int
main( int argc, char* argv[] )
{
    if ( argc < 2 )
    {
        cerr << "Usage: "
        << argv[0]
        << " <local_SIP_port>"
	<< "[<#messages>]"
	<< "[<sendResp|sendNoResp>]"
	<< endl;
        exit( -1 );
    }

    SipTransceiverFilter stack( "perfrecvInvite", atoi( argv[1] ));
    int localPort = atoi(argv[1]);
    stack.setRandomLosePercent(0);

  
    Sptr < SipMsgQueue > sipRcv;
    Sptr < SipMsg > sipMsg;

    
    PerfDuration stat( Data("recvInvite"));  
    
#ifdef PERF_DEBUG
           if (strcmp(getenv("PERF_TEST_PROGRAM"), "test1.2.1") == 0)
           { 
                PerfDuration stat(Data("recvInviteAndDrop"));   
           }   
#endif    
    
    int numMessages = 1; 
    bool sendResp = false;
    
    if (argc >= 3)
    {
	numMessages = atoi(argv[2]);
    }
    
    if (argc >= 4)
    {
	( ( strcmp( argv[3], "sendResp") == 0)? sendResp = true : sendResp = false ); 
    }
    
    
    double sum = 0;    
    
    for (int i = 0; i < numMessages; i++)
    {
	sipRcv = stack.receive(3200);
    
        stat.start();
    
	if ( sipRcv != 0 )
	{
	    sipMsg = sipRcv->back();
	    if ( sipMsg != 0 )
	    {
		Sptr <InviteMsg> inviteMsg;
		
		inviteMsg.dynamicCast(sipMsg); 
		
		if(inviteMsg.getPtr() != 0)
		{
		    cout << inviteMsg->encode().logData() << endl;  
	     
		    if (sendResp)
		    {
			//send 200
			StatusMsg status(*inviteMsg, 200);
			Sptr<SipUrl> sUrl = new SipUrl();
			sUrl->setUserValue("test");
			sUrl->setHost(theSystem.gethostAddress());
			sUrl->setPort(Data(localPort));
			SipContact contact;
			contact.setUrl(sUrl);
			status.setContact(contact);
                        cout << "sending STATUS:" << status.encode().logData() << endl;
			stack.sendReply(status);
		       
                        
		    }
		}
	    }
	}
	stat.stop();
        stat.record(); 
        sum+= stat.average();
	
    }//end for
    
    double avg = sum/numMessages;
    
    cout << "average ticks is" << avg << endl;
    
    return 0;
    
#if 0
    PerformanceDb& perfDb = PerformanceDb::instance();
    
    //this gave a seg fault
    PerfDuration* combinedStat = perfDb.find(stat);
    
    cout << "This stat: " << *combinedStat << endl;  
    cout << "Average: " << combinedStat->average() << endl;
    
    TickCount::calibrate();
    
    cout << "time in seconds: " << combinedStat->average() / TickCount::ticksPerSec << endl;
#endif
}
