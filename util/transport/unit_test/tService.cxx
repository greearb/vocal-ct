
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
static const char* const tService_cxx_Version = 
"$Id: tService.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";
#include "Application.hxx"
#include "Event.hxx"
#include "ControlEvent.hxx"
#include "PollFifo.hxx"
#include "Thread.hxx"
#include <iostream>


using namespace Vocal;
using Vocal::Threads::Thread;
using Vocal::Services::ControlEvent;
using Vocal::Services::Event;

class Module : public Runnable
{
    public:

    	Module() {}
    	~Module() {}

    	ReturnCode  	run();
    	bool	    	processMessages(int numberFdsActive);

    	PollFifo< Sptr<Event> >     &	getFifo()
	{
	    return ( fifo_ );
	}

    private:
    
    	PollFifo< Sptr<Event> >    	fifo_;
};

class tService : public Application
{
    public:
    	tService() {}
	~tService() {}

    	ReturnCode  	    run();	

    	ReturnCode  	    init(int, char**, char **)
	{
	    Thread::init();
	    return ( SUCCESS );
	}
	
	void	    	    uninit()
	{
	    Thread::uninit();
	}

};


ReturnCode  	
Module::run()
{ 
    const string    fn("Module::run");
    ReturnCode	    rc = SUCCESS;
    bool    	    done = false;

    while ( !done )
    {
    	int numberFdsActive = 0;

    	try
	{
            numberFdsActive = fifo_.block();

    	    cout << fn << ": after wait. number fds active = " 
	    	 << numberFdsActive << endl;
	}
	catch ( ... )
	{
	    done = true;
	    rc = -1;
	    continue;
	}

	done = processMessages(numberFdsActive);
    }

    return ( rc );
}

bool	    
Module::processMessages(int numberFdsActive)
{
    const string    fn("Module::processMessages");
    VLog    	    log(fn);

    bool done = false;
    
    while ( fifo_.messageAvailable() )
    {
    	try
	{
	    Sptr<Event> event = fifo_.getNext();

    	    Sptr<ControlEvent> ctrlEvent;
    	    ctrlEvent.dynamicCast(event);

    	    if ( ctrlEvent != 0 )
    	    {
		cout << fn << ": msg = " << *ctrlEvent << endl;

    	    	switch ( ctrlEvent->getType() )
    	    	{
		    case ControlEvent::SHUTDOWN:
		    {
		    	cout << fn << ": Shutdown" << endl;
		    	done = true;
		    	break;
		    }
		    case ControlEvent::START:
		    case ControlEvent::STOP:
		    {
		    	ctrlEvent->setReturnCode(SUCCESS);
			FifoBase< Sptr<Event> > * fifo = ctrlEvent->getReplyFifo();
			if ( fifo )
			{
			    cout << fn << ": Returning message." << endl;
			    
			    // Send the message back.
			    //
			    fifo->add(Sptr<Event>(ctrlEvent));
			}
			else
			{
			    cout << fn << ": Huh? no return fifo." << endl;
			    done = true;
			}
			break;
		    }
		    default:
		    {
		    	cout << fn << ": Huh? bad type" << endl;
			done = true;
		    }
		}
    	    }
	}
	catch ( ... )
	{
	    done = true;
	}
    }

    if ( numberFdsActive )
    {
        fifo_.processProtocols(numberFdsActive);
    }

    return ( done );
}


class tServiceCreator : public ApplicationCreator
{
    Application * create() { return ( new tService ); }
};


ReturnCode
tService::run()
{
    Module  	    	    	module;
    Thread  	    	    	moduleThread(module, "module");
    PollFifo< Sptr<Event> > 	poll;
    
    Sptr<ControlEvent>  event = new ControlEvent(ControlEvent::START, poll);
						
    int id = event->getControlEventId();
    
    cout << "Sending start: " << *event << endl;
    
    module.getFifo().add(Sptr<Event>(event));
    
    bool    done = false;
    int     numberFdsActive;
    
    while ( !done )
    {
        numberFdsActive = poll.block();
    
        if ( poll.messageAvailable() )
	{
	    Sptr<Event> newEvent = poll.getNext();

    	    Sptr<ControlEvent> ctrlEvent;
    	    ctrlEvent.dynamicCast(event);

    	    if  (   ctrlEvent != 0 
	    	&&  ctrlEvent->getControlEventId() == id
	    	)
    	    {
	    	cout << "Received start: " << *ctrlEvent << endl;
	    	done = true;
	    }
	    else
	    {
	    	cout << "Received unknown event." << endl;
	    }
	}

	if ( numberFdsActive )
	{
            poll.processProtocols(numberFdsActive);
	}
    }

    event = new ControlEvent(ControlEvent::STOP, poll);
    
    id = event->getControlEventId();

    module.getFifo().add(Sptr<Event>(event));

    done = false;
        
    while ( !done )
    {
        numberFdsActive = poll.block();
    
        if ( poll.messageAvailable() )
	{
	    Sptr<Event> newEvent = poll.getNext();

    	    Sptr<ControlEvent> ctrlEvent;
    	    ctrlEvent.dynamicCast(event);

    	    if  (   ctrlEvent != 0 
	    	&&  ctrlEvent->getControlEventId() == id
	    	)
    	    {
	    	cout << "Received stop: " << *ctrlEvent << endl;
	    	done = true;
		continue;
	    }
    	    else
	    {	    
	    	cout << "Received unknown event." << endl;
	    }
	}

	if ( numberFdsActive )
	{
            poll.processProtocols(numberFdsActive);
	}
    }
    	    

    event = new ControlEvent(
		    ControlEvent::SHUTDOWN,
		    poll);
    
    module.getFifo().add(Sptr<Event>(event));

    return ( moduleThread.join() );
}

int main(int argc, char ** argv, char ** arge)
{
    tServiceCreator creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}
