#if !defined(VOCAL_SIGNAL_ACTION_HXX)
#define VOCAL_SIGNAL_ACTION_HXX

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


#include "SignalSet.hxx"
#include "Writer.hxx"
#include "NonCopyable.hxx"
#include <string>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to handle signals.
 */
namespace Signals
{


using std::string;


/** An interface class used to respond to a given signal.<br><br>
 *
 *  @see    Vocal::Process::SignalHandler
 *  @see    Vocal::Process::SignalSet
 */
class SignalAction : public Vocal::IO::Writer, public Vocal::NonCopyable
{
    public:

	/** Construct the action that will respond to the signals in the
	 *  given SignalSet.
	 */
        SignalAction(const SignalSet &, const char * name = 0);

	/** Get the set of signals to be responded to.
	 */
        const SignalSet & signalSet() const;


	/** Function that will be called by the SignalHandler 
         *  when a signal in the SignalSet is thrown.
	 */
        void onSignal(int signum, int error, int code);


	/** The name of the class.
	 */
        const string & name() const;


	/** Write the SignalAction to an ostream.
	 */
        ostream & writeTo(ostream &) const;


        /** Returns true if a signal has been caught
         */
        bool signalCaught() const;


	/** The action is reset such that signalCaught will return false.
	 */
	void reset();


    protected:

	/** User callback called by onSignal.
	 */
        virtual void	handleSignal(int signum, int error, int code);
    
    
    private:

        SignalSet   mySignalSet;
        bool        mySignalCaught;
        string      myName;
};


} // namespace Signals
} // namespace Vocal


//#include "DeprecatedSignalAction.hxx"


#endif // !defined(VOCAL_SIGNAL_ACTION_HXX)
