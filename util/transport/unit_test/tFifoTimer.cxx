
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
static const char* const tFifoTimer_cxx_Version = 
    "$Id: tFifoTimer.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
    
#include "Application.hxx"
#include "Fifo.h"
#include "PollFifo.hxx"
#include "Sptr.hxx"
#include "Event.hxx"
#include "Thread.hxx"
#include "SignalHandler.hxx"
#include "VLog.hxx"

using namespace Vocal;
using Vocal::Services::Event;
using Vocal::Threads::Thread;
using Vocal::Process::SignalHandler;
using Vocal::Logging::VLog;

class TestApplication : public Application
{
    public:
    	ReturnCode  	run();
};

class TestApplicationCreator : public ApplicationCreator
{
    Application * create() { return ( new TestApplication ); }
};

int main(int argc, char ** argv, char ** arge)
{
    TestApplicationCreator creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}

class TimerEvent : public Event
{
    public:
    	TimerEvent(int msec, FifoBase< Sptr<Event> > & fifo)
	    :	msec_(msec),
	    	fifo_(fifo),
		id_(0)
	{
	}
	
    	virtual ~TimerEvent() 
	{
	}

    	void	start(Sptr<Event> event)
	{
	    id_ = fifo_.addDelayMs(event, msec_);
	}
	
	
    	FifoEventId 	id()
	{
	    return ( id_ );
	}
	
    	virtual const char* name() const 
	{ 
	    return ( "TimerEvent" ); 
	}

    private:
    	int 	    	    	    	msec_;
	FifoBase< Sptr<Event> > &	fifo_;
	FifoEventId 	    	    	id_;
};

class TimerStart : public Runnable
{
    private:
    	FifoBase< Sptr<Event> > 	&   fifo_;
	
    public:
    	TimerStart(FifoBase< Sptr<Event> > & fifo) : fifo_(fifo) {}
	~TimerStart() {}

	ReturnCode  run()
	{
	    
	    int     twentySec	= 20 * 1000;
	    int     tenSec  	= 10 * 1000;

	    Sptr<TimerEvent> tenSecTimer= new TimerEvent(tenSec, fifo_);
	    Sptr<TimerEvent> twentySecTimer = new TimerEvent(twentySec, fifo_);

    	    tenSecTimer->start(tenSecTimer);
	    twentySecTimer->start(twentySecTimer);
	    
	    return ( SUCCESS );
	}
};

ReturnCode
TestApplication::run()
{
    Thread::init();
    SignalHandler::init();
    VLog::init(LOG_DEBUG);
    
    VLog::on(LOG_INFO);
    VLog::on(LOG_DEBUG);
    
    VLog    	    	    	log;

    {
	Fifo< Sptr<Event> >     fifo;

	TimerStart	    	timerStart(fifo);

	Thread  	    *	timerStartThread = new Thread(timerStart);

	for ( int i = 0; i < 2; i++ )
	{
    	    Sptr<Event> event = fifo.getNext();

	    VDEBUG(log) << "event = " << *event << VDEBUG_END(log);
	}    

	timerStartThread->join();

	delete timerStartThread;
    }

    {
	PollFifo< Sptr<Event> >     fifo;

	TimerStart	    	timerStart(fifo);

	Thread  	    *	timerStartThread = new Thread(timerStart);

	for ( int i = 0; i < 2; i++ )
	{
    	    int msgAvailable = 0;

    	    while ( !msgAvailable )
	    {
		msgAvailable = fifo.block();

		fifo.processProtocols(msgAvailable);
	    }

    	    Sptr<Event> event = fifo.getNext();


	    VDEBUG(log) << "event = " << *event << VDEBUG_END(log);
	}    

	timerStartThread->join();

	delete timerStartThread;
    }

    VLog::uninit();
    SignalHandler::uninit();
    Thread::uninit();

    return ( 0 );
}
