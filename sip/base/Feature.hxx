#ifndef FEATURE_HXX
#define FEATURE_HXX

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

static const char* const Feature_hxx_Version = 
    "$Id: Feature.hxx,v 1.2 2004/05/04 07:31:14 greear Exp $";


#include "Sptr.hxx"
#include "cpLog.h"

#include "State.hxx"
#include "SipProxyEvent.hxx"

namespace Vocal 
{

/** Object Feature
<pre>
<br> Usage of this Class </br>

  Feature is a state container.

FooFeature.hxx:

  class FooFeature : public Vocal::Feature
  {
    public:
         FooFeature();
         virtual ~FooFeature();
         const char* const name() const;
  };

FooFeature.cxx:

  using namespace Vocal;

  FooFeature::FooFeature()
  {
    Sptr < State > mrshlState = new FooState;
    addState( mrshlState );
  }


  FooFeature::~FooFeature()
  {}


  const char* const
  FooFeature::name() const
  {
    return "FooFeature";
  }

</pre>
*/
class Feature: public BugCatcher
{
    public:


        /** Default constructor
         */
        Feature();


        /** Virtual destructor
         */
        virtual ~Feature();


        /** The extender of this class must provide the name of this
         *  class via this abstract method.
         */
        virtual const char* const name() const = 0;


        /** When an event arrives for this feature, each state machine 
         *  associated with this feature process the event, in the order 
         *  the state machines were added to this feature.
         */
        virtual void process(const Sptr < SipProxyEvent > event);


        /** Logging helper method
         */
        void doCpLog() const
        {
            cpLog( LOG_DEBUG, "Feature: %s", name() );
        }


        /** Add a state machine to the feature.
         */
        void addState(const Sptr < State > state);


    protected:


        /** List of states.
         */
        typedef list < Sptr < State > > StateContainer;


        /** Iterator for the states list. Yes, it's poorly named.
         */
        typedef StateContainer::iterator StateIter;


        /** Contains state machines to handle events.
         */
        StateContainer myStates;


    private:
        

        /** Suppress copying
         */
        Feature(const Feature &);


        /** Suppress copying
         */
        const Feature & operator=(const Feature &);
};
 
}

#endif // FEATURE_HXX
