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

static const char* const MicroProxy_cxx_version =
    "$Id: MicroProxy.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <cstdlib>
#include <fstream>

#include "VovidaSipStack.hxx"
#include "SipMsg.hxx"
#include "InviteMsg.hxx"
#include "ByeMsg.hxx"
#include "AckMsg.hxx"
#include "symbols.hxx"

using namespace Vocal;



class Stack
{
    public:
	Stack(int port);

	void loop();

	SipTransceiver& getStack();
	int getPort();

    private:
	SipTransceiverFilter myStack;
	int myPort;
};


Stack::Stack(int port)
    : myStack("", port),
      myPort(port)
{
}


SipTransceiver& 
Stack::getStack()
{
    return myStack;
}


int
Stack::getPort()
{
    return myPort;
}



void 
Stack::loop()
{
    while(1)
    {
	Sptr<SipMsgQueue> queue = myStack.receive();
	// add a via
	Sptr<SipMsg> sipMsg;
	if(queue != 0)
	{
	    sipMsg = queue->back();
	    Sptr<SipMsg> copy = copyPtrtoSptr(&(*sipMsg));
	    assert(sipMsg != 0);
	    // add the via, and send
	    switch(sipMsg->getType())
	    {
	    case SIP_STATUS:
	    {
		// got a reply, so add a via and send it along
		Sptr<StatusMsg> status;
		status.dynamicCast(copy);
		myStack.sendReply(status);
		break;
	    }
	    default:
		Sptr<SipCommand> cmd;
		cmd.dynamicCast(copy);
		myStack.sendAsync(cmd);
		break;
	    }
	}
    }
}


///
int
main( int argc, char* argv[] )
{
    int port = 5065;

    Stack s(port);

    s.loop();

    exit( 0 );
}
