
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


static const char* const tPthreadSignals_cxx_Version = 
    "$Id: tPthreadSignals.cxx,v 1.1 2004/05/01 04:15:38 greear Exp $";


#include <cassert>
#include <cerrno>
#include <string>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/poll.h>
#include <values.h>
#include <sys/time.h>

#include "Application.hxx"
#include "Thread.hxx"
#include "VCondition.h"
#include "VMutex.h"


using namespace Vocal;
using namespace Vocal::Threads;

extern "C"
{
void	handle_sig(int sig, siginfo_t *, void *)
{
    static  int     count = 0;

    cout << "Interrupt: thread = " << Thread::selfId() 
    	 << ", signal = " << sig << endl;

    if ( count++ > 30 )
    {
    	exit(1);
    }
}
}

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

    	tSignal();
	~tSignal();

    	ReturnCode  	    run();	
};

tSignal::tSignal() {}
tSignal::~tSignal() {}

ReturnCode  	
ThreadSignalTester::run()
{ 
    cout << "pid: " << getpid() << endl;

    ReturnCode	    rc = SUCCESS;

    sigset_t	    	hup_mask;
    rc = sigaddset(&hup_mask, SIGHUP);
    assert( rc == 0 );


    rc = pthread_sigmask(SIG_BLOCK, &hup_mask, 0);
    assert( rc == 0 );

    VMutex	    mutex;
    VCondition	    condition;
    
    while ( 1 )
    {
    	rc = pthread_sigmask(SIG_UNBLOCK, &hup_mask, 0);
    	assert( rc == 0 );
	
    	rc = poll(0, 0, -1);
    	assert( rc == -1 && errno == EINTR );	
	
	/*
	    NOTE: pthread_cond_timedwait does not come back return
	    	  when a signal is raised. despite what the man page
		  says. 

    	int relativeTime = 5000; // MAXINT;

	// Relative time is specified, so create the absolute time 
	// of expiration.
	//    
	timeval 	expiresTV;
	int     	rc = gettimeofday(&expiresTV, 0);

	assert( rc == 0 );

	expiresTV.tv_sec += (relativeTime / 1000);
	expiresTV.tv_usec += ((relativeTime % 1000) * 1000);

	timespec	expiresTS;

	expiresTS.tv_sec = expiresTV.tv_sec + ( expiresTV.tv_usec / 1000000 );
	expiresTS.tv_nsec = (expiresTV.tv_usec % 1000000 ) * 1000;

    	vcondition_t *   myId = condition.getId();
	rc = pthread_cond_timedwait(myId, mutex.getId(), &expiresTS);

    	cout << "pthread_cond_timedwait returned: rc = " << rc 
	     << ", " << strerror(rc) << endl;
	     
    	assert( rc == ETIMEDOUT || rc == EINTR );
	*/
	
    	rc = pthread_sigmask(SIG_BLOCK, &hup_mask, 0);

    	assert( rc == 0 );
    }

    return ( rc );
}

class tSignalCreator : public ApplicationCreator
{
    Application * create() { return ( new tSignal ); }
};

ReturnCode
tSignal::run()
{
    cout << "pid: " << getpid() << endl;
    
    ThreadSignalTester	    	signalTester1;
    Thread  	    	    	signalTesterThread1(signalTester1, "signalTester");

    ThreadSignalTester	    	signalTester2;
    Thread  	    	    	signalTesterThread2(signalTester2, "signalTester");

    int rc;
    
    sigset_t	    	hup_mask;
    rc = sigaddset(&hup_mask, SIGHUP);
    assert( rc == 0 );

    rc = pthread_sigmask(SIG_UNBLOCK, &hup_mask, 0);
    assert( rc == 0 );
    
    sigset_t	    	sa_mask;
    rc = sigemptyset(&sa_mask);
    assert( rc == 0 );
    
    struct sigaction	sa;
    
    sa.sa_handler   = 0;
    sa.sa_sigaction = handle_sig;
    sa.sa_mask      = sa_mask;
    sa.sa_flags     = SA_SIGINFO;
    
    rc = sigaction(SIGHUP, &sa, 0);
    assert( rc == 0 );
    
    raise(SIGHUP);
    
    while ( 1 )
    {
    	poll(0, 0, -1);
    }

    return ( signalTesterThread1.join() || signalTesterThread2.join() );
}

int main(int argc, char ** argv, char ** arge)
{
    tSignalCreator  creator;
    Application::creator(&creator);
    return ( Application::main(argc, argv, arge) );
}
    
