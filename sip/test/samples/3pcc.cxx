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

static const char* const t3pcc_cxx_version =
    "$Id: 3pcc.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <cstdlib>
#include <fstream>

#include "VovidaSipStack.hxx"
#include "SipMsg.hxx"
#include "InviteMsg.hxx"
#include "ByeMsg.hxx"
#include "AckMsg.hxx"
#include "symbols.hxx"
#include "SipSdp.hxx"
#include "SystemInfo.hxx"
#include "SipRoute.hxx"

using namespace Vocal;
using namespace Vocal::SDP;

enum CallState
{
    Idle,
    Calling,
    Ringing,
    WaitAck,
//    Answered,
    InCall,
    HangingUp,
    ReInviting,
    Holding,
    OnHold
};

enum CallSide
{
    Callee,
    Caller
};

class Stack;

class Call
{
    public:
	Call(Stack& stack);
	Call(const Call& call);
	Call(Stack& stack, const SipCallId& id);

	/// make a call
	void call(const Data& url);
	/// hold
	void hold();
	/// reinvite
	void reinvite(const SipSdp& sdp);
	/// hangup
	void hangup();
	/// set the SDP to do something
	//	void resetSDP(Sdp);
	/// do something based on this message
	bool gotMsg(Sptr<SipMsgQueue> queue);

	CallState getState();

    private:
	Stack& myStack;
	CallState myState;
	CallSide mySide;
    public:
	Sptr<SipMsgQueue> myQueue;
	Sptr<SipMsg> myLastMsg;
    private:
	Sptr<StatusMsg> myInviteStatus;
	Sptr<InviteMsg> myInvite;
	SipCallId myId;
};


class Stack
{
    public:
	Stack(int port);

	Call* newCall();
	Call* newCall(const SipCallId& id);

	void loop();

	SipTransceiver& getStack();
	int getPort();

    private:
	SipTransceiverFilter myStack;
	int myPort;
	list<Call*> myList;
};

void inCallCallback(Call* call);





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
	cout << "new msg" << endl;
	list<Call*>::iterator i;
	for(i = myList.begin();
	    i != myList.end();
	    ++i)
	{
	    if ((*i)->gotMsg(queue))
		break;
	}
	if(i == myList.end())
	{
	    // no current calls
	    cerr << "call not found, creating new call" << endl;
	    Call* tmp = newCall((*(queue->begin()))->getCallId());
	    tmp->gotMsg(queue);
	}
    }
}


Call*
Stack::newCall()
{
    Call* tmp = new Call(*this);
    myList.push_back(tmp);
    return tmp;
}


Call*
Stack::newCall(const SipCallId& id)
{
    Call* tmp = new Call(*this, id);
    myList.push_back(tmp);
    return tmp;
}


Call::Call(Stack& stack)
    : myStack(stack),
      myState(Idle)
{
}


Call::Call(Stack& stack, const SipCallId& id)
    : myStack(stack),
      myState(Idle),
      myId(id)
{
}


Call::Call(const Call& call)
    :
    myStack(call.myStack),
    myState(call.myState),
    myQueue(call.myQueue)
{
}


void Call::call(const Data& url)
{
    Sptr< SipUrl > toUrl = new SipUrl(url);
    InviteMsg inviteMsg(toUrl, myStack.getPort());

    cout << inviteMsg.encode() << endl;
#if 1
    Sptr<SipSdp> sipSdp;
    sipSdp.dynamicCast ( inviteMsg.getContentData( 0 ) );

    assert (sipSdp != 0);

    if ( sipSdp != 0 )
    {
        SdpSession sdpDesc = sipSdp->getSdpDescriptor();
	sdpDesc.setHold();
        sipSdp->setSdpDescriptor( sdpDesc );
    }

    cout << inviteMsg.encode() << endl;
#endif
    myStack.getStack().sendAsync( inviteMsg );
    myLastMsg = copyPtrtoSptr(&inviteMsg);
    myId = inviteMsg.getCallId();
    myState = Calling;
    myInvite.dynamicCast(myLastMsg);
}


void Call::hold()
{
    Sptr<SipMsg> msg = myQueue->back();
    Sptr<StatusMsg> status;
    status.dynamicCast(msg);
    assert(status != 0);
    InviteMsg reInvite( *status );

    Sptr<SipSdp> sipSdp;
    sipSdp.dynamicCast ( reInvite.getContentData( 0 ) );
    if(sipSdp != 0)
    {
	SdpSession sdpDesc = sipSdp->getSdpDescriptor();
	sdpDesc.setHold();
	sipSdp->setSdpDescriptor( sdpDesc );
    }
    else
    {
	SipSdp sdp;
	SdpSession sdpDesc;

	Data connAddr = theSystem.gethostAddress();
	SdpConnection connection;
        LocalScopeAllocator lo;
	connection.setUnicast(connAddr.getData(lo));
	sdpDesc.setConnection(connection);
	
	SdpMedia* media = new SdpMedia;
	media->setPort(0);

	sdpDesc.addMedia(media);
	srandom((unsigned int)time(NULL));
	int verStr = random();
	
	srandom((unsigned int)time(NULL));
	int sessionStr = random();
	
	sdpDesc.setVersion(verStr);
	sdpDesc.setSessionId(sessionStr);
	sdpDesc.setHold();

	//To further perform any other operation on that particular media
	//item, use loc.
	sdp.setSdpDescriptor(sdpDesc);
	sdp.setUserName(theSystem.getUserName());

	sdp.setSessionName(theSystem.getSessionName());
	sdp.setAddress(theSystem.gethostAddress());
	reInvite.setContentData(&sdp, 0);
    }

    cout << "sending hold:\n" << reInvite.encode() << endl;

    myStack.getStack().sendAsync( reInvite );
    myLastMsg = copyPtrtoSptr(&reInvite);
    myState = Holding;
    myInvite.dynamicCast(myLastMsg);
}


void Call::reinvite(const SipSdp& sdp)
{
    Sptr<SipMsg> msg = myQueue->back();
    Sptr<StatusMsg> status;
    status.dynamicCast(msg);
    assert(status != 0);
    InviteMsg reInvite( *status );
    reInvite.setContentData( &sdp );
//    reInvite.flushViaList();

    cout << "sending reinvite:\n" << reInvite.encode() << endl;

    myStack.getStack().sendAsync( reInvite );
    myLastMsg = copyPtrtoSptr(&reInvite);
    myState = ReInviting;
    myInvite.dynamicCast(myLastMsg);
}


void Call::hangup()
{
    Sptr<StatusMsg> status;
    Sptr<SipCommand> cmd;

    status.dynamicCast(myInviteStatus);
    cmd.dynamicCast(myLastMsg);

    ByeMsg byeMsg;
    
    if(mySide == Caller)
    {
	byeMsg = ByeMsg(*status);
    }
    else
    {
	byeMsg = ByeMsg(*cmd);
    }

    SipFrom from = byeMsg.getFrom();
    from.setTag("");
    byeMsg.setFrom(from);

//    byeMsg.flushViaList();
//    byeMsg.setVia( sipVia, 0 );

//    cmd->getRoute
    byeMsg.copyHeader(*cmd, SIP_ROUTE_HDR);
    if(byeMsg.getNumRoute() > 0)
    {
	SipRoute siproute = byeMsg.getRoute(0);
	byeMsg.removeRoute(0);

	SipRequestLine reqLine = byeMsg.getRequestLine();
	reqLine.setUrl( siproute.getUrl() );
	byeMsg.setRequestLine( reqLine );
    }

    myStack.getStack().sendAsync(byeMsg);
    myLastMsg = copyPtrtoSptr(&byeMsg);
    myState = HangingUp;
}


bool Call::gotMsg(Sptr<SipMsgQueue> sipRcv)
{
    assert ( sipRcv != 0 );

    Sptr<SipMsg> sipMsg;
    Sptr<StatusMsg> statusMsg;
    Sptr<InviteMsg> inviteMsg;
    Sptr<ByeMsg> byeMsg;
    Sptr<SipCommand> sipCommand;
    Data method;
    int status;
    sipMsg = sipRcv->back();
    assert(sipMsg != 0);

    if(sipMsg->getCallId() != myId)
    {
	cerr << "not for me" << endl;
	return false;
    }

    myQueue = sipRcv;
    myLastMsg = sipMsg;

    cout << '\n' << sipMsg->encode() << endl;

    switch(myState)
    {
    case Calling:
    case Ringing:
    case ReInviting:
    case Holding:
	switch(sipMsg->getType())
	{
	case SIP_STATUS:
	    statusMsg.dynamicCast( sipMsg );
	    status = statusMsg->getStatusLine().getStatusCode();
	    method = statusMsg->getCSeq().getMethod();
	    if ( status >= 200 )
	    {
		// Send ACK
		AckMsg ackMsg( *statusMsg );
//		++a;
		myStack.getStack().sendAsync( ackMsg );
		mySide = Caller;
		myLastMsg = copyPtrtoSptr(&ackMsg);
		myInviteStatus = statusMsg;

		if(status == 200)
		{
		    myState = InCall;
		}
		else
		{
		    myState = Idle;
		}
	    }
	    else
	    {
		myState = Ringing;
	    }
	    break;
	}
	break;
    case Idle:
	switch(sipMsg->getType())
	{
	case SIP_INVITE:
	    cout << "got invite" << endl;
	    inviteMsg.dynamicCast( sipMsg );
	    myInvite = inviteMsg;
	    statusMsg = new StatusMsg(*inviteMsg, 200);

	    // Add myself to the contact
	    {
		Sptr<BaseUrl> myUrl;
		myUrl = inviteMsg->getRequestLine().getUrl();
		
		SipContact me;
		me.setUrl( myUrl );
		statusMsg->setNumContact( 0 );    // Clear
		statusMsg->setContact( me );
		myLastMsg = statusMsg;
		myInviteStatus = statusMsg;
	    }

	    myStack.getStack().sendReply( *statusMsg );
	    myState = WaitAck;
	    break;
	case SIP_STATUS:
	    // do nothing
	    break;
	default:
	    sipCommand.dynamicCast( sipMsg );
	    statusMsg = new StatusMsg(*sipCommand, 486);
	    myStack.getStack().sendReply( *statusMsg );
	    break;
	}
	break;
    case WaitAck:
	switch(sipMsg->getType())
	{
	case SIP_ACK:
	    myLastMsg = sipMsg;
	    myState = InCall;
	    mySide = Callee;
	    break;
	}
	break;

    case InCall:
	cout << "in call" << endl;
	switch(sipMsg->getType())
	{
	case SIP_BYE:
	    byeMsg.dynamicCast( sipMsg );
	    statusMsg = new StatusMsg(*byeMsg, 200);
	    myStack.getStack().sendReply( *statusMsg );
	    myState = Idle;
//	    hangupCallback();
	    break;
	}
	break;
    case HangingUp:
	switch(sipMsg->getType())
	{
	case SIP_STATUS:
	    myState = Idle;
	    break;
	case SIP_BYE:
	    byeMsg.dynamicCast( sipMsg );
	    statusMsg = new StatusMsg(*byeMsg, 200);
	    myStack.getStack().sendReply( *statusMsg );
	    myState = Idle;
	    break;
	}
	break;
    }

    if(myState == InCall)
    {
	inCallCallback(this);
//	hangup();
    }

    return true;
}


Call* first = 0;
Call* second = 0;

Sptr<SipSdp> firstSdp;
Sptr<SipSdp> secondSdp;

int has = 0;

void inCallCallback(Call* call)
{
    if(has == 0)
    {
	Sptr<SipMsg> msg = call->myQueue->back();
	
	Sptr<SipContentData> data = msg->getContentData(0);
	assert(data != 0);
	first = call;
	firstSdp.dynamicCast(data);
	assert(firstSdp != 0);
	has++;
	return;
    }
    else if (has == 1)
    {
	second = call;
	// now cross the SDPs
	Sptr<SipMsg> msg = call->myQueue->back();

	Sptr<SipContentData> data = msg->getContentData(0);
	secondSdp.dynamicCast(data);
	assert(secondSdp != 0);
	cout << "got both!" << endl;

	first->hold();
	has++;
	return;
    }
    else if (has == 2)
    {
	second->hold();
	has++;
    }
    else if (has == 3)
    {
	second->reinvite(*firstSdp);
	has++;
    }
    else if (has == 4)
    {
	first->reinvite(*secondSdp);
	has++;
    }
}



///
int
main( int argc, char* argv[] )
{
    int port = 5065;

    if(argc < 3)
    {
	cout << "Usage:\n"
	     << argv[0] 
	     << " sip:first@firstdomain.com sip:second@seconddomain.com\n" 
	     << endl;
	exit(-1);
    }

    const char* url1 = argv[1];
    const char* url2 = argv[2];

    Stack s(port);

    Call* firstCall = s.newCall();

    firstCall->call(url1);

    Call* secondCall = s.newCall();

    secondCall->call(url2);

    s.loop();

    sleep(30);
    exit( 0 );
}
