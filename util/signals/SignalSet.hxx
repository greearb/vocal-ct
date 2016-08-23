#if !defined(VOCAL_SIGNAL_SET_HXX)
#define VOCAL_SIGNAL_SET_HXX

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


#include "global.h"
#include <signal.h>
#include <vector>
#include "Writer.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to handle signals.
 */
namespace Signals
{


using std::vector;


/** A light-weight wrapper for the sigset_t structure.<br><br>
 *  
 *  @see    Vocal::Signals::SignalAction
 *  @see    Vocal::Signals::SignalHandler
 */
class SignalSet : public Vocal::IO::Writer
{
    public:

	/** Default constructor.
	 */
        SignalSet();


	/** Create a signal set with the given signal.
	 */
        SignalSet(int signum);


	/** Create a signal set with the array of signals.
	 */
        SignalSet(int *, size_t);


	/** Crate a signal set with the vector of signals.
	 */
        SignalSet(const vector<int> &);


	/** Copy constructor.
	 */
        SignalSet(const SignalSet &);


	/** Virtual destructor.
	 */
        virtual ~SignalSet() throw (Vocal::SystemException) { };


	/** Equality relational operator.
	 */
        SignalSet & operator=(const SignalSet &);


	/** All signals are set.
	 */
        void	fill();


	/** No signals set.
	 */
        void	empty();


	/** Add the given signal to the set.
	 */
        void	insert(int);


	/** Add the given array of signals to the set.
	 */
        void	insert(int *, size_t);


	/** Add the given vector of signals to the set.
	 */
        void	insert(const vector<int> &);


	/** Remove a signal from the set.
	 */
        void	erase(int);


	/** Remove an array of signals from the set.
	 */
        void	erase(int *, size_t);


	/** Remove a vector of signals from the set.
	 */
        void	erase(const vector<int> &);


   /** Returns true if the given signal is in the set.
    */
   bool	contains(int);


   /** Returns true if all the signals in the array are in the set.
    */
   bool	contains(int *, size_t);


   /** Returne true if all the signals in the vector are in the set.
    */
   bool	contains(const vector<int> &);


	/** Returns the vector of signal set.
	 */
        const vector<int> & signalNumbers() const;


	/** The native signal set.
	 */
        sigset_t signals;


	/** Write the SignalSet to an ostream.
	 */
        virtual ostream & writeTo(ostream &) const;


    private:

        vector<int>     mySignalNumbers;
};


} // namespace Signals
} // namespace Vocal


//#include "DeprecatedSignalSet.hxx"


#endif // !defined(VOCAL_SIGNAL_SET_HXX)
