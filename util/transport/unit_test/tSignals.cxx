
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
static const char* const tSignals_cxx_Version = 
"$Id: tSignals.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "Application.hxx"
#include "Thread.hxx"
#include "SignalHandler.hxx"
#include "PollFifo.hxx"
#include "Fifo.h"
#include <string>


using namespace Vocal;
using Vocal::Threads::Thread;
using Vocal::Process::SignalAction;
using Vocal::Process::SignalSet;
using Vocal::Process::SignalHandler;

class NotMuchAction : public SignalAction
{
    public:

    	NotMuchAction(const SignalSet & set, const string & name) 
	    :   SignalAction(set),
	    	name_(name), 
		count_(0)
	{
	}

	~NotMuchAction() 
	{
	}

    	virtual void	
	onSignal(int signum, int error, int code)
	{
	    cout << name_ 
	    	 << ": thread = " << Thread::selfId()
	    	 << ", received signal = " << signum 
    		 << ", error = " << error << ", code = " << code << endl;
	}

    private:
	string	    	    	name_;
    	int 	    	    	count_;
};
 
class ThreadSignalTester : public Runnable
{
    public:

    	ThreadSignalTester() {}
    	~ThreadSignalTester() {}

    	ReturnCode  	run();
};

class tSignal : public Application
{
    public:

    	tSignal() {}
	~tSignal() {}

    	ReturnCode  	    init(int argc, char ** argv, char ** arge);
	void	    	    uninit();
    	ReturnCode  	    run();	
};


ReturnCode
tSignal::init(int argc, char ** argv, char ** arge)
{
    Thread::init();
    return ( SUCCESS );
}

void
tSignal::uninit()
{
    Thread::uninit();
}

ReturnCode  	
ThreadSignalTester::run()
{ 
    cout << "pid: " << getpid() << endl;

    PollFifo<int>   pollfifo;

    ReturnCode	    rc = SUCCESS;

    SignalSet	    hangup(SIGHUP);
    Thread::self()->signalHandler().block(hangup);

    NotMuchAction   action(hangup, "InThread");
    Thread::self()->signalHandler().add(action);
    
    int count = 0;
    
    while ( count < 5 )
    {
    	try
	{
    	    Thread::self()->signalHandler().unblock(hangup);

            int numberActive = pollfifo.block();
	    
    	    Thread::self()->signalHandler().block(hangup);

	    cout << "pollfifo unblocked: number active = " << numberActive 
	    	 << ", count = " << count++ << endl;
	}
	catch ( ... )
	{
	    continue;
	}
    }

    Thread::self()->signalHandler().remove(action);

    return ( rc );
}

class tSignalsCreator : public ApplicationCreator
{
    Application * create() { return ( new tSignal ); }
};


ReturnCode
tSignal::run()
{
    ThreadSignalTester	signalTester;
    Thread  	    	signalTesterThread(signalTester, "signalTester");

    PollFifo<int>       fifo;

    SignalSet	    	hangup(SIGHUP);
    Thread::self()->signalHandler().block(hangup);

    NotMuchAction   	action(hangup, "InMain");
    Thread::self()->signalHandler().add(action);
    
    int count = 0;
    
    while ( count < 5 )
    {
    	try
	{
    	    Thread::self()->signalHandler().unblock(hangup);

    	    milliseconds_t  forADay = 1000 * 60 * 60; 
	     
            int numberActive = fifo.block(forADay);
	    
    	    Thread::self()->signalHandler().block(hangup);

	    cout << "fifo unblocked: number active = " << numberActive 
	    	 << ", count = " << count++ << endl;
	}
	catch ( ... )
	{
	    continue;
	}
    }

    Thread::self()->signalHandler().remove(action);

    return ( signalTesterThread.join() );
}

int main(int argc, char ** argv, char ** arge)
{
    tSignalsCreator creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}
