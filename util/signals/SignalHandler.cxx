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


static const char* const SignalHandler_cxx_Version = 
    "$Id: SignalHandler.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "global.h"
#include "SignalHandler.hxx"
#include "SignalSet.hxx"
#include "SignalAction.hxx"
#if defined(__APPLE__)
#include "pthread_adds.h"
#endif
#include "VLog.hxx"


using Vocal::Signals::SignalHandler;
using Vocal::Signals::SignalAction;
using Vocal::Signals::SignalSet;
using Vocal::Logging::VLog;


extern "C"
{

#ifdef __APPLE__
static void	    	    
localSignalHandler(int sig)
{
    SignalHandler::signalHandler(sig, NULL, NULL);    
}
#else
static void
localSignalHandler(int sig, siginfo_t * siginfo, void * y)
{
SignalHandler::signalHandler(sig, siginfo, y);
}
#endif
}


SignalHandler::SignalActionRefCountMap	SignalHandler::myActionRefCountMap;
SignalHandler::ThreadSignalMap	    	SignalHandler::myThreadSignalMap;


void
SignalHandler::init()
{
    const string    fn("SignalHandler::init");
    VLog    	    log(fn);
    
    VVERBOSE(log) << fn << VVERBOSE_END(log);
}


void
SignalHandler::uninit()
{
    #if !defined(WIN32) && !defined(__MACH__)
    const string    fn("SignalHandler::uninit");
    VLog    	    log(fn);
    
    VVERBOSE(log) << fn << VVERBOSE_END(log);

    for (   ThreadSignalMap::iterator it = myThreadSignalMap.begin();
    	    it != myThreadSignalMap.end();
	    ++it
	)
    {
    	delete it->second;
	it->second = 0;
    }
    
    myThreadSignalMap.clear();
    #endif // !defined(WIN32)
}


SignalHandler::SignalHandler()
{
    #if !defined(WIN32) && !defined(__MACH__)
    const string    fn("SignalHandler::SignalHandler");
    VLog    	    log(fn);
    
    VVERBOSE(log) << fn << VVERBOSE_END(log);

    ThreadSignalMap::iterator   i = myThreadSignalMap.find(VThread::selfId());

    if ( i == myThreadSignalMap.end() )
    {
    	myThreadSignalMap[VThread::selfId()] = new SignalActionMap;
    }
    else
    {
    	// Can only have, at most, one signal handler per thread.
	//
    	assert( 0 );
    }
    #endif // !defined(WIN32)
}


SignalHandler::~SignalHandler()
{
    #if !defined(WIN32) && !defined(__MACH__)
    const string    fn("SignalHandler::~SignalHandler");
    VLog    	    log(fn);
    
    VVERBOSE(log) << fn << VVERBOSE_END(log);

    ThreadSignalMap::iterator   i = myThreadSignalMap.find(VThread::selfId());

    if ( i != myThreadSignalMap.end() )
    {
        delete i->second;

        myThreadSignalMap.erase(VThread::selfId());
    }
    #endif // !defined(WIN32)
}


void	    	    	    
SignalHandler::add(SignalAction & action)
{
    #if !defined(WIN32) && !defined(__MACH__)
    const string    fn("SignalHandler::add");
    VLog    	    log(fn);
    
    // Find the signal map for this thread. It should exist since it
    // is created during construction.
    //
    SignalActionMap *	sam = myThreadSignalMap[VThread::selfId()];
    assert( sam != 0 );
    
    SignalActionMap &	signalActionMap = *sam;

    VDEBUG(log) << fn << ": action = " << action << VDEBUG_END(log);

    // For each signal, add it to the signal map
    //
    const vector<int> &   signals = action.signalSet().signalNumbers();
    
    size_t size = signals.size();
    
    for ( size_t i = 0; i < size; ++i )
    {
    	int signum = signals[i];
	
    	signalActionMap[signum] = &action;

	// Set the sigaction to the static localSignalHandler(). Note that 
        // there is one sigaction per signal number for the process. Thus the 
	// sigaction may have already been installed for these signal numbers.
	// However they should have been installed to use localSignalHandler(),
	// so reinstalling them doesn't hurt. 
	//
	SignalSet   blocked;
	blocked.fill();

	struct sigaction    	sig_action;

	
#ifdef __APPLE__
        //sig_action.sa_handler 	= SIG_DFL;
        sig_action.sa_handler 	= localSignalHandler;
        sig_action.sa_mask  	= blocked.signals;
	sig_action.sa_flags 	= 0;
#else
	sig_action.sa_sigaction = localSignalHandler;
        sig_action.sa_mask  	= blocked.signals;
	sig_action.sa_flags 	= SA_SIGINFO;
#endif
	

	int rc = sigaction(signum, &sig_action, 0);
	assert ( rc == 0 );

    	// Increment the action reference count only AFTER the sigaction
	// has been set. If this hasn't been added to the map, the lookup 
	// will return 0, and the set will add it to the map.
	//
	myActionRefCountMap[signum] += 1;

    	VDEBUG(log) << fn << ": signum = " << signum 
	    	    << ", reference count = " << myActionRefCountMap[signum]
		    << VDEBUG_END(log);
    }
    #endif // !defined(WIN32)
}


void	    	    	    
SignalHandler::remove(SignalAction & action)
{
    #if !defined(WIN32) && !defined(__MACH__)
    const string    fn("SignalHandler::remove");
    VLog    	    log(fn);
    
    // Find the signal map for this thread. It should exist since it
    // is created during construction.
    //
    SignalActionMap *	sam = myThreadSignalMap[VThread::selfId()];
    assert( sam != 0 );
    
    SignalActionMap &	signalActionMap = *sam;

    VDEBUG(log) << fn << ": action = " << action << VDEBUG_END(log);

    // For each signal, remove it from the signal map.
    //
    const vector<int> &   signals = action.signalSet().signalNumbers();
    
    size_t size = signals.size();
    for ( size_t i = 0; i < size; ++i )
    {
    	int signum = signals[i];
    	// If this isn't the last reference, just decrement the counter.
        //
        SignalActionRefCountMap::iterator j = myActionRefCountMap.find(signum);

	size_t refCount = ( j == myActionRefCountMap.end() ? 0 : j->second );

	if ( refCount != 1 )
	{
            if ( refCount > 1 )
            {
	        myActionRefCountMap[signum] = --refCount;
            }
            else
            {
                VWARN(log) << fn << ": trying to remove signal = " << signum 
                           << ", but it hasn't been added to SignalHandler." 
                           << VWARN_END(log);
            }
	    continue;
	}
	
	// This is the last reference, so reset the sigaction to SIG_DFL
	// and take the signal out of the reference count map. Reset the
	// sigaction first to avoid races caused by inconsistent maps.
	//

    	// No blocked signals. Default constructs to empty.
	//
	SignalSet   noBlocked; 

	struct sigaction    	sig_action;

	sig_action.sa_handler 	= SIG_DFL;
#ifndef __APPLE__
	sig_action.sa_sigaction	= 0;
#endif
	sig_action.sa_mask  	= noBlocked.signals;
	sig_action.sa_flags 	= 0;

	int rc = sigaction(signum, &sig_action, 0);
	assert ( rc == 0 );

    	myActionRefCountMap.erase(signum);	

    	VDEBUG(log) << fn << ": signum = " << signum 
	    	    << ", removing action and using default."
		    << VDEBUG_END(log);

    	// Remove the action from the map AFTER the sigaction has been
	// reset. This avoids the race of a signal firing.
	//
    	signalActionMap.erase(signum);
    }
    #endif // !defined(WIN32)
}


void	    	    	    
SignalHandler::setBlocked(const SignalSet & signalSet)
{
    #if !defined(WIN32) && !defined(__MACH__)
    const string    fn("SignalHandler::setBlocked");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": signalSet = { " << signalSet << " }"
		<< VDEBUG_END(log);

    int rc = pthread_sigmask(SIG_SETMASK, &signalSet.signals, 0);
    assert( rc == 0 );
    #endif // !defined(WIN32)
}


void	    	    	    
SignalHandler::block(const SignalSet & signalSet)
{
    #if !defined(WIN32) && !defined(__MACH__)
    const string    fn("SignalHandler::block");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": signalSet = { " << signalSet << " }"
		<< VDEBUG_END(log);

    int rc = pthread_sigmask(SIG_BLOCK, &signalSet.signals, 0);
    assert( rc == 0 );
    #endif // !defined(WIN32)
}


void	    	    	    
SignalHandler::unblock(const SignalSet & signalSet)
{
    #if !defined(WIN32) && !defined(__MACH__)
    const string    fn("SignalHandler::unblock");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": signalSet = { " << signalSet << " }"
		<< VDEBUG_END(log);

    int rc = pthread_sigmask(SIG_UNBLOCK, &signalSet.signals, 0);
    assert( rc == 0 );
    #endif // !defined(WIN32)
}


void	    	    	    
SignalHandler::setGlobalBlocked(const SignalSet & signalSet)
{
    #if !defined(WIN32) && !defined(__MACH__)
    const string    fn("SignalHandler::setGlobalBlocked");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": signalSet = { " << signalSet << " }"
		<< VDEBUG_END(log);

    int rc = sigprocmask(SIG_SETMASK, &signalSet.signals, 0);
    assert( rc == 0 );
    #endif // !defined(WIN32)
}


void	    	    	    
SignalHandler::globalBlock(const SignalSet & signalSet)
{
    #if !defined(WIN32) && !defined(__MACH__)
    const string    fn("SignalHandler::globalBlock");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": signalSet = { " << signalSet << " }"
		<< VDEBUG_END(log);

    int rc = sigprocmask(SIG_BLOCK, &signalSet.signals, 0);
    assert( rc == 0 );
    #endif // !defined(WIN32)
}


void	    	    	    
SignalHandler::globalUnblock(const SignalSet & signalSet)
{
    #if !defined(WIN32) && !defined(__MACH__)
    const string    fn("SignalHandler::globalUnblock");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": signalSet = { " << signalSet << " }"
		<< VDEBUG_END(log);
    
    int rc = sigprocmask(SIG_UNBLOCK, &signalSet.signals, 0);
    assert( rc == 0 );
    #endif // !defined(WIN32)
}


void	    	    
SignalHandler::signalHandler(int sig, siginfo_t * siginfo, void *)
{
    #if !defined(WIN32) && !defined(__MACH__)
    assert( siginfo != 0 );

    ThreadSignalMap::iterator   i = myThreadSignalMap.find(VThread::selfId());

    if ( i == myThreadSignalMap.end() )
    {
        assert( 0 );
        return;
    }
        
    SignalActionMap &	signalActionMap = *(i->second);

    int signum = ( siginfo == 0 ? sig : siginfo->si_signo );

    SignalActionMap::iterator j = signalActionMap.find(signum);
    
    if ( j == signalActionMap.end() )
    {
        assert( 0 );
        return;
    }
    
    SignalAction    *	action = j->second;
    
    // Do the callback.
    //
    if ( action != 0 )
    {
    	action->onSignal(signum, 
                         siginfo ? siginfo->si_errno : 0, 
                         siginfo ? siginfo->si_code  : 0);
    }
    
    // We shouldn't get here since we are reference counting the actions. 
    //
    else
    {
    	assert( 0 );
    }
    #endif // !defined(WIN32) 
}
