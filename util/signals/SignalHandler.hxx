  #if !defined(VOCAL_SIGNAL_HANDLER_HXX)
#define VOCAL_SIGNAL_HANDLER_HXX

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


static const char* const SignalHandler_hxx_Version = 
    "$Id: SignalHandler.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "NonCopyable.hxx"
#include "global.h"
#include "SignalSet.hxx"
#include "SignalAction.hxx"
#include "VThread.hxx"
#include <csignal>
#include <map>

#if defined(__MACH__)
#include <signal.h>
typedef int siginfo_t;
#endif

/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to handle signals.
 */
namespace Signals
{


using std::map;

#ifdef __APPLE__
    typedef struct {
        int	si_signo;
        int	si_errno;
        int 	si_code;
    } siginfo_t;
#endif

/** Support for unix type signals in a mutlithreaded or single threaded
 *  environment.<br><br>
 *
 *  @see    Vocal::Signals::SignalAction
 *  @see    Vocal::Signals::SignalSet
 *  @see    Vocal::Application
 *  @see    Vocal::Threads::Thread
 */
class SignalHandler : public Vocal::NonCopyable
{

    public:


	/** Global initialization. Should be called in Application::init().
	 */
    	static void     init();


	/** Global uninitialization. Should be called in Application::unit().
	 */
	static void 	uninit();
	

	/** Only one signal handler per thread can be created.
	 *  It needs to be created in the context of the thread.
	 */
    	SignalHandler();
	

	/** Virtual destructor.
	 */
	virtual     ~SignalHandler();


    	/** Adds an action to be handled for this thread.
	 */
    	void	    add(SignalAction &);
	

	/** Removes an action to be handled for this thread.
	 */
    	void	    remove(SignalAction &);


	/** Sets the blocked signals for this thread.
	 */
    	void	    setBlocked(const SignalSet &);


	/** Adds the signals to those blocked for this thread.
	 */
    	void	    block(const SignalSet &);


	/** Removes the signals from those blocked for this thread.
	 */
	void	    unblock(const SignalSet &);


	/** Sets the blocked signals for this process.
	 */
    	static void setGlobalBlocked(const SignalSet &);


	/** Adds the signals to those blocked for this process.
	 */
    	static void globalBlock(const SignalSet &);


	/** Removes the signals from those blocked for this process.
	 */
	static void globalUnblock(const SignalSet &);


	/** The actual signal handling routine.
	 */
    	static void signalHandler(int, siginfo_t *, void *);


    private:


    	typedef map<int, size_t>                    SignalActionRefCountMap;
    	typedef map<int, SignalAction *>    	    SignalActionMap;
    	typedef map<vthread_t, SignalActionMap *>   ThreadSignalMap;


    	static SignalActionRefCountMap	    	    myActionRefCountMap;
    	static ThreadSignalMap     	    	    myThreadSignalMap;
};


} // namespace Signals
} // namespace Vocal


//#include "DeprecatedSignalHandler.hxx"


#endif // !defined(VOCAL_SIGNAL_HANDLER_HXX)
