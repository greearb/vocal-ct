
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

static const char* const tProtocol_cxx_Version = 
    "$Id: tProtocol.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include <unistd.h>
#include "Application.hxx"
#include "Thread.hxx"
#include "VLog.hxx"
#include "SignalHandler.hxx"
#include "TPKTClientProtocol.hxx"
#include "TPKTClientSocket.hxx"
#include "PollFifoService.hxx"
#include "PollFifo.hxx"
#include "Sptr.hxx"
#include "Event.hxx"


using namespace Vocal;
using Vocal::Threads::Thread;
using Vocal::Process::SignalHandler;
using Vocal::Logging::VLog;
using Vocal::Transport::TPKTClientProtocol;
using Vocal::Transport::TPKTClientSocket;
using Vocal::Services::Event;
using Vocal::Services::PollFifoService;
using Vocal::IO::FileDescriptor;


class Q931ProtocolBase : public TPKTClientProtocol
{

    public:
    
        Q931ProtocolBase(
            PollFifoService 	    	&   service,
    	    Sptr<TPKTClientSocket> 	    q931Socket,
    	    const char      	    	*   name = 0
    	);

    	virtual ~Q931ProtocolBase();

        virtual const FileDescriptor& getFileDescriptor() const;

    	virtual TPKTClientSocket& getTPKTClientSocket();
		
    	virtual void onTPKTArrived(vector<u_int8_t>& message);

    private:

    	PollFifoService     	    &	service_;
        Sptr<TPKTClientSocket>	    	q931Socket_;

};

class Q931Protocol : public Q931ProtocolBase
{
    public:
    
        Q931Protocol(
            PollFifoService 	    	&   service,
    	    Sptr<TPKTClientSocket> 	    q931Socket
    	)
	    :	Q931ProtocolBase(service, q931Socket)
    	{
	}
	

    	virtual ~Q931Protocol()
	{
    	}

};

class tApplication : public Application
{
    public:

    	tApplication() {}
	~tApplication() {}

    	ReturnCode  	    init(int argc, char ** argv, char ** arge);
	void	    	    uninit();
    	ReturnCode  	    run();	
};


Q931ProtocolBase::Q931ProtocolBase(
    PollFifoService 	    	&   service,
    Sptr<TPKTClientSocket> 	    q931Socket,
    const char      	    	*   name
)
    :	TPKTClientProtocol(name),
    	service_(service),
	q931Socket_(q931Socket)
{
    awaitIncoming(true);
    service_.registerProtocol(*this);
}


Q931ProtocolBase::~Q931ProtocolBase()
{
    service_.unregisterProtocol(*this);
}


const FileDescriptor &
Q931ProtocolBase::getFileDescriptor() const
{
    return ( *q931Socket_ );
}


TPKTClientSocket & 
Q931ProtocolBase::getTPKTClientSocket()
{
    return ( *q931Socket_ );
}


void 
Q931ProtocolBase::onTPKTArrived(vector<u_int8_t>& message)
{
}

ReturnCode
tApplication::init(int argc, char ** argv, char ** arge)
{
    Thread::init();
    VLog::init(LOG_DEBUG);
    SignalHandler::init();

    VLog::on(LOG_VERBOSE);
    VLog::on(LOG_DEBUG);
    VLog::on(LOG_INFO);
        
    return ( SUCCESS );
}

void
tApplication::uninit()
{
    SignalHandler::uninit();
    VLog::uninit();
    Thread::uninit();
}

class tApplicationCreator : public ApplicationCreator
{
    Application * create() { return ( new tApplication ); }
};


ReturnCode
tApplication::run()
{
    PollFifoService 	    service;

    Sptr<TPKTClientSocket>  q931Socket0 = new TPKTClientSocket;
    Sptr<Q931Protocol>      q931Protocol0 = new Q931Protocol(service, q931Socket0);

    Sptr<TPKTClientSocket>  q931Socket1 = new TPKTClientSocket;
    Sptr<Q931Protocol>      q931Protocol1 = new Q931Protocol(service, q931Socket1);
    
    Sptr<TPKTClientSocket>  q931Socket2 = new TPKTClientSocket;
    Sptr<Q931Protocol>      q931Protocol2 = new Q931Protocol(service, q931Socket2);
    
    Sptr<TPKTClientSocket>  q931Socket3 = new TPKTClientSocket;
    Sptr<Q931Protocol>      q931Protocol3 = new Q931Protocol(service, q931Socket3);
    
    Sptr<TPKTClientSocket>  q931Socket4 = new TPKTClientSocket;
    Sptr<Q931Protocol>      q931Protocol4 = new Q931Protocol(service, q931Socket4);
    
    Sptr<TPKTClientSocket>  q931Socket5 = new TPKTClientSocket;
    Sptr<Q931Protocol>      q931Protocol5 = new Q931Protocol(service, q931Socket5);
    
    Sptr<TPKTClientSocket>  q931Socket6 = new TPKTClientSocket;
    Sptr<Q931Protocol>      q931Protocol6 = new Q931Protocol(service, q931Socket6);
    
    Sptr<TPKTClientSocket>  q931Socket7 = new TPKTClientSocket;
    Sptr<Q931Protocol>      q931Protocol7 = new Q931Protocol(service, q931Socket7);
    
    Sptr<TPKTClientSocket>  q931Socket8 = new TPKTClientSocket;
    Sptr<Q931Protocol>      q931Protocol8 = new Q931Protocol(service, q931Socket8);
    
    sleep(1);

    q931Protocol0 = 0;
    q931Protocol1 = 0;
    q931Protocol2 = 0;
    q931Protocol3 = 0;
    q931Protocol4 = 0;
    q931Protocol5 = 0;
    q931Protocol6 = 0;
    q931Protocol7 = 0;
    q931Protocol8 = 0;

    return ( 0 );
}

int main(int argc, char ** argv, char ** arge)
{
    tApplicationCreator creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}
