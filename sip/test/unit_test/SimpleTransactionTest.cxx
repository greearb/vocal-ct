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

static const char* const SimpleTransactionTest_cxx_Version =
    "$Id: SimpleTransactionTest.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipTransceiver.hxx"
#include "SipMsg.hxx"
#include <cassert>
#include "cpLog.h"
#include "InviteMsg.hxx"
#include "Verify.hxx" 
#include "ThreadIf.hxx"
#include "StatusMsg.hxx"
#include "AckMsg.hxx"
#include "SipVia.hxx"
#include "SystemInfo.hxx"

string host_addr = Vocal::theSystem.gethostAddress();

using namespace Vocal;

bool gotAck = false;

class Sender : public ThreadIf
{
    public:
	Sender()
	    :ThreadIf(0, VTHREAD_PRIORITY_DEFAULT, VTHREAD_STACK_SIZE_DEFAULT)
	{
	}
	void thread();

};

void Sender::thread()
{
    SipTransceiver trans(6000, host_addr, "", "test", 8088);

    Sptr<SipUrl> toUrl = new SipUrl(string("sip:bko@127.0.0.1:8086"), host_addr);
    InviteMsg imsg( toUrl, host_addr, 8088 );
    trans.sendAsync(imsg);
    while(!isShutdown())
    {
	Sptr<SipMsgQueue> msgQ = trans.receive(1000);
	if(msgQ != 0)
	{
	    cout << "*************** GOT MSG **************" << endl;
	    Sptr<SipMsg> msg = *(msgQ->rbegin());
	    if(msg->getType() == SIP_STATUS)
	    {
		// form an ACK
		Sptr<StatusMsg> status;
		status.dynamicCast(msg);
		AckMsg ack (*status, host_addr);
		ack.flushViaList();
		
		SipVia via = imsg.getVia(0);
		ack.setVia(via, 0);
//            ack.setRouteList(myRouteList);
//            ack.removeRoute(0);
		
		Sptr<SipUrl> sUrl;
		sUrl.dynamicCast(imsg.getRequestLine().getUrl());
		SipRequestLine rLine(SIP_ACK, sUrl, host_addr);
		ack.setRequestLine(rLine);
		
		trans.sendAsync( ack );
	    }
	}
    }
    
}


class Receiver : public ThreadIf
{
    public:
	Receiver()
	    :ThreadIf(0, VTHREAD_PRIORITY_DEFAULT, VTHREAD_STACK_SIZE_DEFAULT)
	{
	}
	void thread();

};


void Receiver::thread()
{
    SipTransceiver trans(6000, host_addr, "", "test2", 8086);
    while(!isShutdown())
    {
	Sptr<SipMsgQueue> msgQ = trans.receive(1000);
	if(msgQ != 0)
	{
	    Sptr<SipMsg> msg = *(msgQ->rbegin());
	    if(msg->getType() == SIP_INVITE)
	    {
		// reply
		Sptr<InviteMsg> invite;
		invite.dynamicCast(msg);
		StatusMsg reply( *invite, 302 );
		trans.sendReply( reply );
	    }
	    if(msg->getType() == SIP_ACK)
	    {
		// then it's good, isn't it?
		gotAck = true;
	    }
	}
    }
}


int main()
{
    cpLogSetPriority(LOG_DEBUG_STACK);

    Sender X;
    Receiver Y;

    X.run();
    Y.run();

    sleep(3);
    X.shutdown();
    Y.shutdown();

    X.join();
    Y.join();

    test_verify(gotAck == true);

    return test_return_code(1);
}
